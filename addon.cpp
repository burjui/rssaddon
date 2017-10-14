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

using namespace std;
using namespace mgr_log;
using namespace mgr_xml;

#define BINARY_NAME "rssaddon"
MODULE(BINARY_NAME);

void appendMetadata(XmlNode &xml);
void appendToolBar(XmlNode &metadata);
void appendTableDescription(XmlNode &metadata);
void appendColumnDescription(XmlNode &coldata, const char *name);
void appendMessages(XmlNode &xml);
void appendMessage(XmlNode &messages, const char *name, const char *text);
void appendTableParameters(XmlNode &xml);
void appendTableRow(XmlNode &xml, const RssItem &item);

int ISP_MAIN(int argc, char *argv[])
{
	mgr_log::Init(BINARY_NAME);
	LogInfo("Invoked as %s", argv[0]);

	Xml outXml;
	auto outXmlRoot = outXml.GetRoot();
	outXmlRoot.SetProp("func", BINARY_NAME);
	appendMetadata(outXmlRoot);
	appendMessages(outXmlRoot);
	appendTableParameters(outXmlRoot);

	ifstream exchangeFile(EXCHANGE_XML_FILE_PATH);
	Debug("%s exchange file '%s'", exchangeFile.is_open() ? "opened" : "cannot open", EXCHANGE_XML_FILE_PATH);
	if (exchangeFile.is_open())
	{
		auto exchangeXml = Xml(exchangeFile);
		string itemsXpath = "/doc/";
		itemsXpath += RssItem::NODE_NAME;
		auto itemNodes = exchangeXml.GetNodes(itemsXpath);
		Debug("found %zu items", itemNodes.size());
		for_each(itemNodes.begin(), itemNodes.end(), [&outXmlRoot] (const XmlNode &itemNode) {
			auto rssItem = RssItem(itemNode);
			appendTableRow(outXmlRoot, rssItem);
		});
	}

	cout << outXml.Str(true);

	return EXIT_SUCCESS;
}

void appendMetadata(XmlNode &xml)
{
	auto metadata = xml.AppendChild("metadata")
		.SetProp("name", BINARY_NAME)
		.SetProp("type", "list")
		.SetProp("key", "name")
		.SetProp("mgr", "billmgr");
	appendToolBar(metadata);
	appendTableDescription(metadata);
}

void appendToolBar(XmlNode &metadata)
{
	auto toolbar = metadata.AppendChild("toolbar")
		.SetProp("view", "buttontext");
	auto toolgrp = toolbar.AppendChild("toolgrp")
		.SetProp("name", "new");
	// Fetch button
	toolgrp.AppendChild("toolbtn")
		.SetProp("name", "new")
		.SetProp("func", "rssfetcher")
		.SetProp("type", "refresh")
		.SetProp("img", "t-new")
		.SetProp("sprite", "yes");
}

void appendTableDescription(XmlNode &metadata)
{
	auto coldata = metadata.AppendChild("coldata");
	appendColumnDescription(coldata, "title");
	appendColumnDescription(coldata, "pubDate");
	appendColumnDescription(coldata, "link");
}

void appendColumnDescription(XmlNode &coldata, const char *name)
{
	coldata.AppendChild("col")
		.SetProp("name", name)
		.SetProp("type", "data")
		.SetProp("sort", "alpha");
}

void appendMessages(XmlNode &xml)
{
	auto messages = xml.AppendChild("messages")
		.SetProp("name", BINARY_NAME);
	appendMessage(messages, "title", "Test");
	appendMessage(messages, "hint_name", "Название");
	appendMessage(messages, "hint_value", "Значение");
	appendMessage(messages, "short_new", "Создать");
	appendMessage(messages, "short_delete", "Удалить");
	appendMessage(messages, "hint_delete", "Удалить");
	appendMessage(messages, "hint_new", "Создать");
	appendMessage(messages, "name", "Имя");
	appendMessage(messages, "value", "Значение");
	appendMessage(messages, "hint_export", "Сохранить в CSV");
	appendMessage(messages, "hint_selectall", "Выделить все элементы списка");
	appendMessage(messages, "hint_reloadlist", "Обновить данные");
	appendMessage(messages, "hint_print", "Открыть версию для печати");
	appendMessage(messages, "hint_autoupdate_stop", "Отменить автообновление текущего списка");
	appendMessage(messages, "hint_takefavorite", "Добавить в избранное меню");
	appendMessage(messages, "hint_takeunfavorite", "Убрать из избранного меню");
	appendMessage(messages, "msg_tsetting", "Настроить вид таблицы");
}

void appendMessage(XmlNode &messages, const char *name, const char *text)
{
	messages.AppendChild("msg")
		.SetProp("name", name)
		.SetContent(text);
}

void appendTableParameters(XmlNode &xml)
{
	auto tparams = xml.AppendChild("tparams");
	tparams.AppendChild("out", "devel");
	tparams.AppendChild("func", BINARY_NAME);
}

void appendTableRow(XmlNode &xml, const RssItem &item)
{
	auto elem = xml.AppendChild("elem");
	elem.AppendChild("title", item.title);
	elem.AppendChild("pubDate", item.pubDate);
	elem.AppendChild("link", item.link);
}
