#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <mgr/mgrxml.h>
#include <mgr/mgrlog.h>
#include <ispbin.h>
#include "rssitem.hpp"
#include "common.h"
#include <api/module.h>
#include <api/action.h>

namespace {

MODULE("rssaddon");
	
using namespace std;
using namespace mgr_log;
using namespace mgr_xml;
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

MODULE_INIT(rssaddon, "")
{
	new RssAddon();
}

} // namespace
