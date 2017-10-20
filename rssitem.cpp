#include "rssitem.hpp"

RssItem::RssItem(const mgr_xml::XmlNode &item):
    guid(item.FindNode("guid").Str()),
    title(item.FindNode("title").Str()),
    pubDate(item.FindNode("pubDate").Str()),
    link(item.FindNode("link").Str())
{
}
