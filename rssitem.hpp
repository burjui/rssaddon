#ifndef __RSSITEM_H__
#define	__RSSITEM_H__

#include <mgr/mgrxml.h>

struct RssItem
{
	string guid;
	string title;
	string pubDate;
	string link;

	RssItem(const mgr_xml::XmlNode &item);
};

#endif // __RSSITEM_H__
