#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <mgr/mgrxml.h>

using namespace std;

int main(const int argc, const char *const *argv)
{
	ifstream xml("/tmp/test.xml");
	cout << xml.rdbuf();

	return EXIT_SUCCESS;
}
