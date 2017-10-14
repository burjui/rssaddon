#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <mgr/mgrxml.h>
#include <mgr/mgrlog.h>
#include <ispbin.h>
#include <curl/curl.h>

using namespace std;
using namespace mgr_log;
using namespace mgr_xml;

#define BINARY_NAME "rssfetcher"
MODULE(BINARY_NAME);

stringstream rssResponse;
extern "C" size_t responseWriter(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	auto totalSize = size * nmemb;
	rssResponse.write(ptr, totalSize);
	return totalSize;
}

int ISP_MAIN(const int argc, char *argv[])
{
	mgr_log::Init(BINARY_NAME, L_DEBUG);
	LogInfo("Invoked as %s", argv[0]);

	auto curl = curl_easy_init();
	if (!curl) {
		return EXIT_FAILURE;
	}
	curl_easy_setopt(curl, CURLOPT_URL, "http://feeds.rucast.net/radio-t");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseWriter);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    rssResponse.seekg(0, ios::end);
    const auto responseSize = static_cast<size_t>(rssResponse.tellg());
    rssResponse.seekg(0, ios::beg);
    Debug("Got RSS response (%zu bytes)", static_cast<size_t>(responseSize));

    auto rssXml = Xml(rssResponse);
    auto items = rssXml.GetNodes("/rss/channel/item");
    Debug("Fetched %zu items", items.size());
    for_each(items.begin(), items.end(), [] (const XmlNode &item) {
		Debug("item '%s'", item.FindNode("title").Str().c_str());
	});

	cout << Xml().Str();

	return EXIT_SUCCESS;
}
