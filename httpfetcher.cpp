#include <string>
#include <mgr/mgrerr.h>
#include "httpfetcher.hpp"

using namespace std;
using namespace mgr_err;

namespace
{
	template <typename ...Args> Error error(Args... args)
	{
		return Error("rssfetcher", args...);
	}

	void curlCheck(CURLcode code, const string &object)
	{
		if (code != CURLE_OK)
			throw error(object, curl_easy_strerror(code));
	}
}

extern "C" size_t responseWriter(char *ptr, size_t size, size_t nmemb, void *userdata);

HttpFetcher::HttpFetcher()
{
	curl = curl_easy_init();
	if (!curl)
		throw error("curl_easy_init()");
	curlCheck(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseWriter), "CURLOPT_WRITEFUNCTION");
}

HttpFetcher::~HttpFetcher()
{
	curl_easy_cleanup(curl);
}

void HttpFetcher::fetch(const char *url, std::ostream &output)
{
	curlCheck(curl_easy_setopt(curl, CURLOPT_URL, url), "CURLOPT_URL");
	curlCheck(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output), "CURLOPT_WRITEDATA");
	curlCheck(curl_easy_perform(curl), "curl_easy_perform()");
	curlCheck(curl_easy_setopt(curl, CURLOPT_WRITEDATA, nullptr), "CURLOPT_WRITEDATA");
}

extern "C" size_t responseWriter(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	auto response = reinterpret_cast<ostream*>(userdata);
	auto totalSize = size * nmemb;
	response->write(ptr, totalSize);
	return totalSize;
}
