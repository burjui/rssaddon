#ifndef __TABLE_H__
#define __TABLE_H__

#include <mgr/mgrdb_struct.h>

struct TABLE_MODULE;

class RssTable : public mgr_db::IdTable
{
public:
	mgr_db::StringField guid;
	mgr_db::StringField title;
	mgr_db::StringField pubDate;
	mgr_db::StringField link;

	RssTable();
};

#endif // __TABLE_H__
