#include "table.hpp"
#include "db.hpp"
#include "module.h"

RssTable::RssTable():
	mgr_db::IdTable("rss"),
	guid(this, "guid"),
	title(this, "title"),
	pubDate(this, "pubDate"),
	link(this, "link")
{
}

namespace
{

PROPER_MODULE_INIT(TABLE_MODULE, DB_MODULE)
{
	dbCache->Register<RssTable>();
}

}
