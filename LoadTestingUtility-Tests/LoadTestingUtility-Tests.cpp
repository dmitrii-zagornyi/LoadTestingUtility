#include "pch.h"
#include "CppUnitTest.h"

#include "../LoadTestingUtility/LoadTestingUtility.cpp"
#include <charconv>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace LoadTestingUtilityTests
{
	TEST_CLASS(LoadTestingUtilityTests)
	{
	private:
		string urlValid = "https://www.sberbank.ru";
		string urlInvalid = "https://www.google.com1234";
	public:
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
			CURL* curl = curl_easy_init();

			map<long, vector<double>> response;

			SendGetRequest(curl, urlInvalid, response);

			Assert::IsTrue(response.size() == 1);
			Assert::IsTrue(response[response.begin()->first].size() == 1);
			Assert::IsTrue(response.begin()->first < 0);
			Assert::IsTrue(response[response.begin()->first][0] == 0.0);

			curl_easy_cleanup(curl);
		}
		TEST_METHOD(SendGetRequest_CurlNotInit)
		{
			map<long, vector<double>> response;

			SendGetRequest(NULL, urlInvalid, response);

			Assert::IsTrue(response.size() == 1);
			Assert::IsTrue(response[response.begin()->first].size() == 1);
			Assert::IsTrue(response.begin()->first < 0);
			Assert::IsTrue(response[response.begin()->first][0] == 0.0);
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
	TEST_CLASS(LoadTestingUtilityTestsUI)
	{
	private:
		string urlValid = "https://www.sberbank.ru";
	public:
		TEST_METHOD(CheckMain_NoArgs)
		{
			vector<const char*> argv;
			argv.push_back("LoadTestingUtility.exe");

			int result = main(int(argv.size()), argv.data());

			Assert::AreEqual(result, 1);
		}
		TEST_METHOD(CheckMain_FourArgs)
		{
			vector<const char*> argv;
			argv.push_back("LoadTestingUtility.exe");
			argv.push_back("LoadTestingUtility.exe");
			argv.push_back("LoadTestingUtility.exe");
			argv.push_back("LoadTestingUtility.exe");

			int result = main(int(argv.size()), argv.data());

			Assert::AreEqual(result, 1);
		}
		TEST_METHOD(CheckMain_ValidArgs)
		{
			vector<const char*> argv;
			argv.push_back("LoadTestingUtility.exe");
			argv.push_back(urlValid.c_str());
			string number = to_string(10);
			argv.push_back(number.c_str());

			int result = main(int(argv.size()), argv.data());

			Assert::AreEqual(result, 0);
		}
	};
}
