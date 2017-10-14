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
#include "rssitem.hpp"

using namespace std;
using namespace mgr_log;
using namespace mgr_xml;

#define BINARY_NAME "rssfetcher"
MODULE(BINARY_NAME);

static stringstream rssResponse;
extern "C" size_t responseWriter(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	auto totalSize = size * nmemb;
	rssResponse.write(ptr, totalSize);
	return totalSize;
}

int ISP_MAIN(const int argc, char *argv[])
{
	mgr_log::Init(BINARY_NAME);
	LogInfo("Invoked as %s", argv[0]);

	auto curl = curl_easy_init();
	if (!curl)
	{
		LogError("curl_easy_init() failed");
		return EXIT_FAILURE;
	}
	const char *RSS_FEED_URL = "http://feeds.rucast.net/radio-t";
	curl_easy_setopt(curl, CURLOPT_URL, RSS_FEED_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseWriter);
    auto curlResponseCode = curl_easy_perform(curl);
    if (curlResponseCode != CURLE_OK)
    {
		LogError("curl_easy_perform() failed for '%s': %s", RSS_FEED_URL, curl_easy_strerror(curlResponseCode));
		return EXIT_FAILURE;
	}
    curl_easy_cleanup(curl);

	{
		rssResponse.seekg(0, ios::end);
		const auto responseSize = static_cast<size_t>(rssResponse.tellg());
		rssResponse.seekg(0, ios::beg);
		Debug("Got RSS response (%zu bytes)", static_cast<size_t>(responseSize));
	}

	Xml outXml;
	auto outXmlRoot = outXml.GetRoot();
	{
		auto rssXml = Xml(rssResponse);
		auto items = rssXml.GetNodes("/rss/channel/item");
		Debug("Fetched %zu items", items.size());
		for_each(items.begin(), items.end(), [&outXmlRoot] (const XmlNode &itemXml) {
			Debug("item '%s'", itemXml.FindNode("title").Str().c_str());
			auto item = RssItem(itemXml);
			item.appendToXmlNode(outXmlRoot);
		});
	}
	ofstream outXmlFile("/tmp/out.xml");
	outXml.Save(outXmlFile, true);

	cout << Xml().Str();

	return EXIT_SUCCESS;
}
