#include "pch.h"
#include "CppUnitTest.h"

#include "../LoadTestingUtility/LoadTestingUtility.cpp"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace LoadTestingUtilityTests
{
	TEST_CLASS(LoadTestingUtilityTests)
	{
	private:
		string urlValid = "https://www.sberbank.ru";
		string urlInvalid = "https://www.google.com1234";
	public:
		TEST_METHOD(CheckStatus_Ok)
		{
			::CheckStatus(CURLE_OK);
		}
		TEST_METHOD(CheckStatus_Fail)
		{
			auto func = [this] { ::CheckStatus(CURLE_UNSUPPORTED_PROTOCOL); };
			Assert::ExpectException<std::exception>(func);
		}
		TEST_METHOD(SendGetRequest_Default)
		{
			CURL* curl = curl_easy_init();

			map<long, vector<double>> response;

			SendGetRequest(curl, urlValid, response);

			Assert::IsTrue(response.size() == 1);
			Assert::IsTrue(response[response.begin()->first].size() == 1);
			Assert::IsTrue(response[response.begin()->first][0] < 2.0);

			curl_easy_cleanup(curl);
		}
		TEST_METHOD(SendGetRequest_NotEmptyMapAnotherResponse)
		{
			CURL* curl = curl_easy_init();

			map<long, vector<double>> response;

			response[2001].push_back(5.0);

			SendGetRequest(curl, urlValid, response);

			Assert::IsTrue(response.size() == 2);
			Assert::IsTrue(response[response.begin()->first].size() == 1);
			Assert::IsTrue(response[response.begin()->first][0] < 2.0);
			Assert::IsTrue(response[2001].size() == 1);
			Assert::IsTrue(response[2001][0] == 5.0);

			curl_easy_cleanup(curl);
		}
		TEST_METHOD(SendGetRequest_IncorrectUrl)
		{
			auto func = [this] {
				CURL* curl = curl_easy_init();
				map<long, vector<double>> response;
				SendGetRequest(curl, urlInvalid, response); };
			Assert::ExpectException<std::exception>(func);
		}
		TEST_METHOD(SendGetRequest_CurlNotInit)
		{
			auto func = [this] {
				map<long, vector<double>> response;
				SendGetRequest(NULL, urlValid, response); };
			Assert::ExpectException<std::exception>(func);
		}
		TEST_METHOD(SendBatchGetRequest_Default)
		{
			constexpr unsigned int requestsCount = 3;

			map<long, vector<double>> response;

			SendBatchGetRequest(urlValid, requestsCount, response);

			Assert::IsTrue(response.size() == 1);
			Assert::IsTrue(response[response.begin()->first].size() == requestsCount);
			for (unsigned int i = 0; i < requestsCount; i++)
			{
				Assert::IsTrue(response[response.begin()->first][i] < 2.0);
			}
		}
		TEST_METHOD(SendBatchRequest_Zero)
		{
			constexpr unsigned int requestsCount = 0;

			map<long, vector<double>> response;

			SendBatchGetRequest(urlValid, requestsCount, response);

			Assert::IsTrue(response.size() == 0);
		}
		TEST_METHOD(SendParallelBatchGetRequest_ZeroThreadZeroRequests)
		{
			constexpr unsigned int threadsCount = 0;
			constexpr unsigned int requestsCount = 0;

			map<long, vector<double>> response;

			SendParallelBatchGetRequest(urlValid, threadsCount, requestsCount, response);

			Assert::IsTrue(response.size() == 0);
		}
		TEST_METHOD(SendParallelBatchGetRequest_OneThreadZeroRequests)
		{
			constexpr unsigned int threadsCount = 1;
			constexpr unsigned int requestsCount = 0;

			map<long, vector<double>> response;

			SendParallelBatchGetRequest(urlValid, threadsCount, requestsCount, response);

			Assert::IsTrue(response.size() == 0);
		}
		TEST_METHOD(SendParallelBatchGetRequest_OneThreadOneRequest)
		{
			constexpr unsigned int threadsCount = 1;
			constexpr unsigned int requestsCount = 1;

			map<long, vector<double>> response;

			SendParallelBatchGetRequest(urlValid, threadsCount, requestsCount, response);

			Assert::IsTrue(response.size() == 1);
			Assert::IsTrue(response[response.begin()->first].size() == threadsCount * requestsCount);
			Assert::IsTrue(response[response.begin()->first][0] < 2.0);
		}
		TEST_METHOD(SendParallelBatchGetRequest_OneThreadManyRequest)
		{
			constexpr unsigned int threadsCount = 1;
			constexpr unsigned int requestsCount = 3;

			map<long, vector<double>> response;

			SendParallelBatchGetRequest(urlValid, threadsCount, requestsCount, response);

			Assert::IsTrue(response.size() == 1);
			Assert::IsTrue(response[response.begin()->first].size() == threadsCount * requestsCount);

			for (unsigned int i = 0; i < requestsCount; i++)
			{
				Assert::IsTrue(response[response.begin()->first][i] < 2.0);
			}
		}
		TEST_METHOD(SendParallelBatchGetRequest_ManyThreadOneRequest)
		{
			constexpr unsigned int threadsCount = 3;
			constexpr unsigned int requestsCount = 1;

			map<long, vector<double>> response;

			SendParallelBatchGetRequest(urlValid, threadsCount, requestsCount, response);

			Assert::IsTrue(response.size() == 1);
			Assert::IsTrue(response[response.begin()->first].size() == threadsCount * requestsCount);

			for (unsigned int i = 0; i < requestsCount; i++)
			{
				Assert::IsTrue(response[response.begin()->first][i] < 2.0);
			}
		}
		TEST_METHOD(SendParallelBatchGetRequest_ManyThreadManyRequest)
		{
			constexpr unsigned int threadsCount = 3;
			constexpr unsigned int requestsCount = 2;

			map<long, vector<double>> response;

			SendParallelBatchGetRequest(urlValid, threadsCount, requestsCount, response);

			Assert::IsTrue(response.size() == 1);
			Assert::IsTrue(response[response.begin()->first].size() == threadsCount * requestsCount);

			for (unsigned int i = 0; i < requestsCount; i++)
			{
				Assert::IsTrue(response[response.begin()->first][i] < 2.0);
			}
		}
	};
}
