#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#define CURL_STATICLIB
#include <curl\curl.h>


using namespace std;


void CheckStatus(CURLcode hr)
{
    if (hr != CURLE_OK)
    {
        throw runtime_error(curl_easy_strerror(hr));
    }
}

void SendGetRequest(CURL* curl, string url, map<long, vector<double>>& response)
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

    response[response_code].push_back(elapsed);
}

void SendBatchGetRequest(string url, unsigned int requestsCount, map<long, vector<double>>& response)
{
    CURL* curl = curl_easy_init();

    for (unsigned int i = 0; i < requestsCount; i++)
    {
        SendGetRequest(curl, url, response);
    }

    curl_easy_cleanup(curl);
}

void SendParallelBatchGetRequest(string url, unsigned int threadsCount, unsigned int requestsCount, map<long, vector<double>>& response)
{
    vector<pair<thread*, map<long, vector<double>>*>> batchGetRequests;

    for (unsigned int i = 0; i < threadsCount; i++)
    {
        map<long, vector<double>>* pThreadResponse = new map<long, vector<double>>;
        thread* pThread = new std::thread(&SendBatchGetRequest, url, requestsCount, ref(response));
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
                response[it->first].push_back(it->second[j]);
            }
        }
    }

    for (unsigned int i = 0; i < threadsCount; i++)
    {
        delete batchGetRequests[i].first;
        delete batchGetRequests[i].second;
    }
}


int main()
{
    constexpr unsigned int threadsCount = 2;
    constexpr unsigned int requestsCount = 100;

    map<long, vector<double>> response;

    SendParallelBatchGetRequest("https://habr.com/ru/all/", threadsCount, requestsCount, response);

    return 0;
}