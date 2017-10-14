#ifndef __RSSITEM_H__
#define	__RSSITEM_H__

#include <mgr/mgrxml.h>

struct RssItem
{
    static const char *NODE_NAME;

	string guid;
	string title;
	string pubDate;
	string link;

	RssItem(const mgr_xml::XmlNode &item);
    void appendToXmlNode(mgr_xml::XmlNode &node) const;
};

#endif // __RSSITEM_H__
