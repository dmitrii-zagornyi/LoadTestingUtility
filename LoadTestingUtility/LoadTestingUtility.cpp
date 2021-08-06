#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#define CURL_STATICLIB
#include <curl/curl.h>


using namespace std;


#define CheckStatus(hr) if (hr != CURLE_OK) { responses[hr * (-1)].push_back(0); return; }


size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp)
{
    return size * nmemb;
}

void SendGetRequest(CURL* curl, string url, map<long, vector<double>>& responses)
{
    CURLcode hr;

    hr = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    CheckStatus(hr);

    hr = curl_easy_perform(curl);
    CheckStatus(hr);

    long response_code;
    double elapsed;
    hr = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    CheckStatus(hr);

    hr = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    CheckStatus(hr);

    responses[response_code].push_back(elapsed);
}

void SendBatchGetRequest(string url, unsigned int requestsCount, map<long, vector<double>>& responses)
{
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);  // Disable console output https://stackoverflow.com/questions/3572397/lib-curl-in-c-disable-printing

    for (unsigned int i = 0; i < requestsCount; i++)
    {
        SendGetRequest(curl, url, responses);
    }

    curl_easy_cleanup(curl);
}

void SendParallelBatchGetRequest(string url, unsigned int threadsCount, unsigned int requestsCount, map<long, vector<double>>& responses)
{
    CURLcode hr = curl_global_init(CURL_GLOBAL_ALL);  // Lazy initialising work incorrect in multithreading mode https://curl.se/libcurl/c/threadsafe.html
    if (hr != CURLE_OK)
    {
        throw runtime_error(curl_easy_strerror(hr));
    }

    vector<pair<thread*, map<long, vector<double>>*>> batchGetRequests;

    for (unsigned int i = 0; i < threadsCount; i++)
    {
        map<long, vector<double>>* pThreadResponse = new map<long, vector<double>>;
        thread* pThread = new std::thread(&SendBatchGetRequest, url, requestsCount, ref(*pThreadResponse));
        batchGetRequests.push_back(make_pair(pThread, pThreadResponse));
    }

    for (unsigned int i = 0; i < threadsCount; i++)
    {
        batchGetRequests[i].first->join();

        map<long, vector<double>>* threadResponse = batchGetRequests[i].second;
        for (auto it = threadResponse->begin(); it != threadResponse->end(); it++)
        {
            for (unsigned int j = 0; j < it->second.size(); j++)
            {
                responses[it->first].push_back(it->second[j]);
            }
        }
    }

    for (unsigned int i = 0; i < threadsCount; i++)
    {
        delete batchGetRequests[i].first;
        delete batchGetRequests[i].second;
    }
}

void printStatistics(const map<long, vector<double>>& responses)
{
    cout << "Statistics" << endl;
    double totalTime = 0;
    size_t requestsCount = 0;
    for (auto it = responses.begin(); it != responses.end(); it++)
    {
        string requestStatus;
        if (it->first < 0)
        {
            requestStatus = curl_easy_strerror(CURLcode(it->first * (-1)));
        }
        else
        {
            requestStatus = to_string(it->first);
        }
        cout << requestStatus << " - " << it->second.size() << " requests" << endl;
        for (unsigned int number = 0; number < it->second.size(); number++)
        {
            totalTime += it->second[number];
            requestsCount++;
        }
    }
    double averageTime = (totalTime / requestsCount) * 1000.0;

    stringstream stream;
    stream << fixed << setprecision(0) << averageTime;
    string averageTimeString = stream.str();

    cout << endl << "Average time: " << averageTimeString << " ms" << endl;
}


int main(int argc, const char* argv[])
{
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <url> <requests count per thread>" << endl;

        return 1;
    }
    string url = argv[1];
    unsigned int requestsCount = (unsigned int)stoul(argv[2]);
    constexpr unsigned int threadsCount = 10;

    map<long, vector<double>> responses;

    SendParallelBatchGetRequest(url, threadsCount, requestsCount, responses);
    printStatistics(responses);

    return 0;
}
