#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <mgr/mgrxml.h>
#include <mgr/mgrlog.h>
#include <ispbin.h>

using namespace std;
using namespace mgr_log;

#define BINARY_NAME "rssfetcher"
MODULE(BINARY_NAME);

int ISP_MAIN(const int argc, char *argv[])
{
	mgr_log::Init(BINARY_NAME);
	LogInfo("Invoked as %s", argv[0]);

	// We don't output any data, just make the core happy
	cout << mgr_xml::Xml().Str();

	return EXIT_SUCCESS;
}
