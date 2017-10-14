#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iostream>
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

int ISP_MAIN(int argc, char *argv[])
{
	mgr_log::Init(BINARY_NAME);
	LogInfo("Invoked as %s", argv[0]);

	ifstream exchangeFile(EXCHANGE_XML_FILE_PATH);

	Xml outXml;
	auto outXmlRoot = outXml.GetRoot();
	outXmlRoot.SetProp("func", BINARY_NAME);
	appendMetadata(outXmlRoot);

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
