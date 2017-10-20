#include "db.hpp"
#include "table.hpp"
#include "rssitem.hpp"
#include "httpfetcher.hpp"
#include "log.hpp"
#include "module.h"
#include <sstream>
#include <mgr/mgrlog.h>
#include <api/action.h>

namespace
{

using namespace std;
using namespace isp_api;
using namespace mgr_log;

class RssFetch: public Action
{
public:
	RssFetch(): Action("rss.fetch", MinLevel(lvAdmin))
	{
	}

	virtual void Execute(Session &session) const override
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
		auto rssXml = Xml(rssResponse);
		auto items = rssXml.GetNodes("/rss/channel/item");
		Debug("Fetched %zu items", items.size());
		for_each(items.begin(), items.end(), [] (const XmlNode &itemXml)
		{
			Debug("item '%s'", itemXml.FindNode("title").Str().c_str());
			auto item = RssItem(itemXml);
			saveToDb(item);
		});
	}

private:
	static void saveToDb(const RssItem &item)
	{
		auto table = dbCache->Get<RssTable>();
		if (!table->guid.Lookup(item.guid))
		{
			LogInfo("Creating a record for %s", item.guid.c_str());
			table->New();
		}
		else
			LogInfo("Updating %s", item.guid.c_str());
		table->guid = item.guid;
		table->title = item.title;
		table->pubDate = item.pubDate;
		table->link = item.link;
		table->Post();
	}
};

struct fetch;
PROPER_MODULE_INIT(fetch, TABLE_MODULE, DB_MODULE)
{
	new RssFetch();
}

} // namespace
