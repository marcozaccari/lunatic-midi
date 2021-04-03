#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

namespace zini
{
extern string resourcefile;

string zini_readstring(string fsection, string fitem, string def_notfound);
int zini_readinteger(string fsection, string fitem, int def_notfound);
bool zini_readboolean(string fsection, string fitem, bool def_notfound);
int zini_readninteger(string fsection, string fitem, unsigned n, int def_notfound);
bool zini_replacestring(string FSection, string FItem, string NewS);

bool zini_init();
}

namespace zstrings
{
	string ucase(string s);
	string lcase(string s);
	string trim(string s);
	int pos(string substr, string s);
	string copy(string s, int start, int count=0);
	int str2int(string s, bool force_hex = false);
	string int2str(int i);
	string int2hex(int i, int digits);
	string extract_filename(string s);
	std::vector<std::string> explode(char delim, const std::string &s);
}
