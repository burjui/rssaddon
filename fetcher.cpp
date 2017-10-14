#include <sstream>
#include <fstream>
#include <iostream>
#include <mgr/mgrxml.h>
#include <mgr/mgrlog.h>
#include <ispbin.h>
#include <curl/curl.h>
#include <cstdio>
#include <cstdlib>

using namespace std;
using namespace mgr_log;

#define BINARY_NAME "rssfetcher"
MODULE(BINARY_NAME);

stringstream rssResponse;
extern "C" size_t curl_writefunction(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	auto totalSize = size * nmemb;
	rssResponse.write(ptr, totalSize);
	return totalSize;
}

int ISP_MAIN(const int argc, char *argv[])
{
	mgr_log::Init(BINARY_NAME);
	LogInfo("Invoked as %s", argv[0]);

	// We don't output any data, just make the core happy
	//cout << mgr_xml::Xml().Str();

	auto curl = curl_easy_init();
	if (!curl) {
		return EXIT_FAILURE;
	}
	curl_easy_setopt(curl, CURLOPT_URL, "http://feeds.rucast.net/radio-t");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writefunction);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    rssResponse.seekg(0, ios::end);
    printf("### RESPONSE SIZE %zu bytes\n", static_cast<size_t>(rssResponse.tellg()));
    rssResponse.seekg(0, ios::beg);

    //cout << rssResponse.rdbuf();

    auto rssXml = mgr_xml::Xml(rssResponse);
    cout << rssXml.Str();

	return EXIT_SUCCESS;
}
