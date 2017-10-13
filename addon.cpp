#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <mgr/mgrxml.h>
#include <mgr/mgrlog.h>
#include <ispbin.h>

using namespace std;
using namespace mgr_log;

#define BINARY_NAME "rssaddon"
MODULE(BINARY_NAME);

int ISP_MAIN(int argc, char *argv[])
{
	mgr_log::Init(BINARY_NAME);
	LogInfo("Invoked as %s", argv[0]);

	ifstream xml("/tmp/test.xml");
	cout << xml.rdbuf();

	return EXIT_SUCCESS;
}
