#include "rssitem.hpp"

const char *RssItem::NODE_NAME = "rssitem";

RssItem::RssItem(const mgr_xml::XmlNode &item):
    guid(item.FindNode("guid").Str()),
    title(item.FindNode("title").Str()),
    pubDate(item.FindNode("pubDate").Str()),
    link(item.FindNode("link").Str())
{
}

void RssItem::appendToXmlNode(mgr_xml::XmlNode &node) const
{
    auto item = node.AppendChild(NODE_NAME);
    item.AppendChild("guid", guid);
    item.AppendChild("title", title);
    item.AppendChild("pubDate", pubDate);
    item.AppendChild("link", link);
}
