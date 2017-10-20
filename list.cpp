#include "db.hpp"
#include "table.hpp"
#include "module.h"
#include <api/dbaction.h>
#include <string>
#include <algorithm>
#include <functional>
#include <vector>

namespace
{

struct list;
PROPER_MODULE_INIT(list, TABLE_MODULE)
{
	new isp_api::TableIdListAction<RssTable>("rss.list", isp_api::AccessAdmin, *dbCache);
};

} // namespace
