#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <mgr/mgrxml.h>
#include <mgr/mgrlog.h>
#include <mgr/mgrdb_struct.h>
#include <ispbin.h>
#include <api/module.h>
#include <api/action.h>
#include "common.h"
#include "rssitem.hpp"
#include "httpfetcher.hpp"
#include <cassert>

namespace {

#define MOD_NAME "rssaddon"
MODULE(MOD_NAME);

using namespace std;
using namespace mgr_log;
using namespace mgr_xml;
using namespace mgr_db;
using namespace isp_api;

mgr_db::Cache *db_cache;

class RssTable : public IdTable
{
public:
	StringField guid;
	StringField title;
	StringField pubDate;
	StringField link;

	RssTable():
		IdTable("rss"),
		guid(this, "guid"),
		title(this, "title"),
		pubDate(this, "pubDate"),
		link(this, "link")
	{
	}
};

shared_ptr<RssTable> rssTable()
{
	return db_cache->Get<RssTable>();
}

class RssList: public ListAction
{
public:
	RssList(): ListAction("rss.list", MinLevel(lvAdmin))
	{
		assert(rssTable());
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

class RssFetch: public Action
{
public:
	RssFetch(): Action("rss.fetch", MinLevel(lvAdmin))
	{
	}

	void Execute(Session &session) const override
	{
		(void) session;

		// Fetch the RSS feed into rssResponse via libcurl
		stringstream rssResponse;
		HttpFetcher().fetch("http://feeds.rucast.net/radio-t", rssResponse);

		// Log the response size
		const auto responseSize = static_cast<int>(rssResponse.tellg());
		Debug("Got RSS response (%d bytes)", responseSize);
		rssResponse.seekg(0, ios::beg);

		// Parse items from the RSS data and output them into the exchange file
		Xml outXml;
		auto outXmlRoot = outXml.GetRoot();
		auto rssXml = Xml(rssResponse);
		auto items = rssXml.GetNodes("/rss/channel/item");
		Debug("Fetched %zu items", items.size());
		for_each(items.begin(), items.end(), [&outXmlRoot] (const XmlNode &itemXml)
		{
			Debug("item '%s'", itemXml.FindNode("title").Str().c_str());
			auto item = RssItem(itemXml);
			item.appendToXmlNode(outXmlRoot);
			saveToDb(item);
		});
		db_cache->Commit();
		ofstream outXmlFile(EXCHANGE_XML_FILE_PATH);
		outXml.Save(outXmlFile, true);
	}

private:
	static void saveToDb(const RssItem &item)
	{
		auto table = rssTable();
		if (!table->guid.Lookup(item.guid))
		{
			LogInfo("Creating a record for %s", item.guid.c_str());
			table->New();
		}
		LogInfo("Updating %s", item.guid.c_str());
		table->guid = item.guid;
		table->title = item.title;
		table->pubDate = item.pubDate;
		table->link = item.link;
		table->Post();
	}
};

MODULE_INIT(db, "")
{
	ConnectionParams params;
	params.type = "mysql";
	params.db = "billmgr";
	params.user = "coremgr";
	params.password = "H9iJCm6ocP";
	params.client = "rssaddon";
	db_cache = new Cache(params);
}

MODULE_INIT(rssaddon, "db")
{
	mgr_log::Init(MOD_NAME);
	db_cache->Register<RssTable>();
	new RssList();
	new RssFetch();
}

} // namespace
