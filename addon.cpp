#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <mgr/mgrxml.h>
#include <mgr/mgrlog.h>
#include <mgr/mgrerr.h>
#include <ispbin.h>
#include "rssitem.hpp"
#include "common.h"
#include <api/module.h>
#include <api/action.h>
#include <curl/curl.h>

namespace {

MODULE("rssaddon");
	
using namespace std;
using namespace mgr_log;
using namespace mgr_xml;
using namespace mgr_err;
using namespace isp_api;

class RssAddon: public ListAction 
{
public:
	RssAddon(): ListAction("rssaddon", MinLevel(lvAdmin))
	{
	}
	
	void List(Session &session) const override
	{
		ifstream exchangeFile(EXCHANGE_XML_FILE_PATH);
		Debug("%s exchange file '%s'", exchangeFile.is_open() ? "opened" : "cannot open", EXCHANGE_XML_FILE_PATH);
		if (exchangeFile.is_open())
		{
			auto exchangeXml = Xml(exchangeFile);
			string itemsXpath = "/doc/";
			itemsXpath += RssItem::NODE_NAME;
			auto itemNodes = exchangeXml.GetNodes(itemsXpath);
			Debug("found %zu items", itemNodes.size());
			for_each(itemNodes.begin(), itemNodes.end(), [&session] (const XmlNode &itemNode)
			{
				auto rssItem = RssItem(itemNode);
				showRssItem(session, rssItem);
			});
		}
	}

private:
	static void showRssItem(Session &session, const RssItem &item)
	{
		auto elem = session.NewElem();
		elem.AppendChild("title", item.title);
		elem.AppendChild("pubDate", item.pubDate);
		elem.AppendChild("link", item.link);
	}
};

extern "C" size_t responseWriter(char *ptr, size_t size, size_t nmemb, void *userdata);

class HttpFetcher
{
public:
	HttpFetcher()
	{
		curl = curl_easy_init();
		if (!curl)
			throw error("curl_easy_init()");
	}
	
	~HttpFetcher()
	{
		curl_easy_cleanup(curl);
	}
	
	void fetch(ostream &output)
	{
		const char *RSS_FEED_URL = "http://feeds.rucast.net/radio-t";
		curlCheck(curl_easy_setopt(curl, CURLOPT_URL, RSS_FEED_URL), "CURLOPT_URL");
		curlCheck(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseWriter), "CURLOPT_WRITEFUNCTION");
		curlCheck(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output), "CURLOPT_WRITEDATA");
		curlCheck(curl_easy_perform(curl), "curl_easy_perform()");
	}
	
private:
	CURL *curl;
	
	static void curlCheck(CURLcode code, const string &object)
	{
		if (code != CURLE_OK)
			throw error(object, curl_easy_strerror(code));
	}
	
	template <typename ...Args> static Error error(Args... args)
	{
		return Error("rssfetcher", args...);
	}
};


extern "C" size_t responseWriter(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	auto response = reinterpret_cast<ostream*>(userdata);
	auto totalSize = size * nmemb;
	response->write(ptr, totalSize);
	return totalSize;
}


class Fetcher: public Action 
{
public:
	Fetcher(): Action("rssfetcher", MinLevel(lvAdmin))
	{
	}
	
	void Execute(Session &session) const override
	{
		(void) session;
		
		// Fetch the RSS feed into rssResponse via libcurl
		stringstream rssResponse;
		HttpFetcher().fetch(rssResponse);

		// Log the response size
		const auto responseSize = static_cast<int>(rssResponse.tellg());
		Debug("Got RSS response (%d bytes)", responseSize);
		rssResponse.seekg(0, ios::beg);

		// Parse items from the RSS data and output them into the exchange file
		Xml outXml;
		auto outXmlRoot = outXml.GetRoot();
		{
			auto rssXml = Xml(rssResponse);
			auto items = rssXml.GetNodes("/rss/channel/item");
			Debug("Fetched %zu items", items.size());
			for_each(items.begin(), items.end(), [&outXmlRoot] (const XmlNode &itemXml)
			{
				Debug("item '%s'", itemXml.FindNode("title").Str().c_str());
				auto item = RssItem(itemXml);
				item.appendToXmlNode(outXmlRoot);
			});
		}
		ofstream outXmlFile(EXCHANGE_XML_FILE_PATH);
		outXml.Save(outXmlFile, true);
	}
};

MODULE_INIT(rssaddon, "")
{
	new RssAddon();
	new Fetcher();
}

} // namespace
