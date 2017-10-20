#include "db.hpp"
#include "module.h"
#include <mgr/mgrdb.h>

using namespace mgr_db;

JobCache *dbCache;

namespace
{

PROPER_MODULE_INIT(DB_MODULE)
{
	ConnectionParams params;
	params.host = "localhost";
	params.type = "mysql";
	params.db = "billmgr";
	params.user = "coremgr";
	params.password = "H9iJCm6ocP";
	params.client = "rssaddon";
	dbCache = new JobCache(params);
	isp_api::RegisterComponent(dbCache);
}

}
