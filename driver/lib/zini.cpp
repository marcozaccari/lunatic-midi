#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <vector>

#ifdef WIN32
    #include <winsock2.h>
    #include <Windows.h>
    #include <Winbase.h>
#endif

using namespace std;

namespace zstrings
{
	string ucase(string s);
	string lcase(string s);
	string trim(string s);
	int pos(string substr, string s);
	string copy(string s, int start, int count);
	int str2int(string s, bool force_hex);
	string int2str(int i);
	string int2hex(int i, int digits);
	string extract_filename(string s);
	std::vector<std::string> &explode(char delim, const std::string &s, std::vector<std::string> &elems);
	std::vector<std::string> explode(char delim, const std::string &s);
}

using namespace zstrings;

namespace zstrings
{

string ucase(string s){
	for (unsigned k = 0; k < s.length(); k++) s[k] = toupper(s[k]);
	return s;
}

string lcase(string s){
	for (unsigned k = 0; k < s.length(); k++) s[k] = tolower(s[k]);
	return s;
}

string trim(string s){
	int start, end;
	string sout;

	start = 0;
	while ((start <= (int)s.length()) && (s[start] <= 0x020)) start++;
	end = s.length()-1;
	while ((end > 0) && (s[end] <= 0x020)) end--;
	for (int k = start; k <= end; k++) sout += s[k];
	return sout;
}

int pos(string substr, string s){
	string::size_type loc = s.find(substr, 0);
	if (loc != string::npos)
	   return loc;
	   else
	   return -1;
}

string copy(string s, int start, int count){
	string sout;
	int len;
	
	len = s.length();
	
	if ((len == 0) || (start >= len)) 
		return "";
	
	if ((count <= 0) || (start+count > len))
		count = len - start;
		
	for (int k = start; k < start+count; k++) 
		sout += s[k];
		
	return sout;
}

int str2int(string s, bool force_hex){
   if (force_hex || (zstrings::pos("0x", zstrings::lcase(s)) >= 0))
	   return strtol(s.c_str(), NULL, 16);
	else
		return atoi(s.c_str());
}

string int2str(int i){
	char cs[128];
	string s;
	sprintf(cs, "%i", i);
	s = cs;
	return s;
}

string int2hex(int i, int digits){
	char cs[128];
	string s;
	sprintf(cs, "%0*X", digits, i);
	s = cs;
	return s;
}


string extract_filename(string s){
	if (s.length() == 0) return "";
	if ((pos("/", s)<0) && (pos("\\", s)<0)) return s;
	int k = s.length()-1;
	while ((k >= 0) && (s[k] != '/') && (s[k] != '\\')) k--;
	return copy(s, k+1, s.length());
}

std::vector<std::string> &explode(char delim, const std::string &s, std::vector<std::string> &elems){
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> explode(char delim, const std::string &s){
    std::vector<std::string> elems;
    explode(delim, s, elems);
    return elems;
}

}

namespace zini
{

std::string resourcefile;

string item(string s)
{
        s = copy(s, 0, pos("=", s));
        s = ucase(trim(s));
	return s;
}

string section(string s)
{
        if (pos("[", s) == -1) return "";
        s = copy(s, pos("[",s)+1, pos("]",s)-1);
        s = ucase(trim(s));
        return s;
}

string param(string s)
{
        s = copy(s, pos("=", s)+1, s.length()-pos("=", s));
        s = trim(s);
	return s;
}

string readstring(string fsection, string fitem, string def_notfound)
{
	string line;
	char c[256];
	ifstream file;

	file.open(resourcefile.c_str());
	if (!file) return def_notfound;
	fsection = ucase(trim(fsection));
	fitem = ucase(trim(fitem));

	line = "";
	while ((section(line) != fsection) && (!file.eof()))
              {
              file.getline(c, 256);
              line = c;
	      line = trim(line);
              }
	if (file.eof())
	   {
	   file.close();
	   return def_notfound;
	   }

	while ((item(line) != fitem) && !file.eof())
              {
              file.getline(c, 256);
              line = c;
	      line = trim(line);
              }
	file.close();
	if ((file.eof()) && (item(line) != fitem)) return def_notfound;
	line = param(line);
	return line;
}

int readinteger(string fsection, string fitem, int def_notfound = -1)
{
	string line;
	char c[256];
	ifstream file;

	file.open(resourcefile.c_str());
	if (!file) return def_notfound;
	fsection = trim(ucase(fsection));
        fitem = trim(ucase(fitem));

	line = "";
	while ((section(line) != fsection) && (!file.eof()))
              {
              file.getline(c, 256);
              line = c;
	      line = trim(line);
              }
	if (file.eof())
	   {
	   file.close();
	   return def_notfound;
	   }

	while ((item(line) != fitem) && !file.eof())
              {
              file.getline(c, 256);
              line = c;
	      line = trim(line);
              }
	file.close();
	if ((file.eof()) && (item(line) != fitem)) return def_notfound;
	line = param(line);
	return atoi(line.c_str());
}

int readninteger(string fsection, string fitem, unsigned n, int def_notfound = -1)
{
	string line;
	char c[256];
	ifstream file;

	file.open(resourcefile.c_str());
	if (!file) return def_notfound;
	fsection = trim(ucase(fsection));
        fitem = trim(ucase(fitem));

	line = "";
	while ((section(line) != fsection) && (!file.eof()))
              {
              file.getline(c, 256);
              line = c;
	      line = trim(line);
              }
	if (file.eof())
	   {
	   file.close();
	   return def_notfound;
	   }

	while ((item(line) != fitem) && !file.eof())
              {
              file.getline(c, 256);
              line = c;
	      line = trim(line);
              }
	file.close();
	if ((file.eof()) && (item(line) != fitem)) return def_notfound;
	line = param(line);

	int i, nsep, startpos, endpos;
	startpos = 0;
	nsep = 0;
	while ((startpos < (int)line.length()) && (nsep < (int)(n-1)))
	      {
	      if (line[startpos] == ',') nsep++;
	      startpos++;
	      }
	endpos = startpos;
	while ((endpos < (int)line.length()) && (line[endpos] != ',')) endpos++;
	line = trim(copy(line, startpos, endpos-startpos));
	i = atoi(line.c_str());
	return i;
}

bool readboolean(string fsection, string fitem, bool def_notfound = false)
{
	string line;
	char c[256];
	ifstream file;

	file.open(resourcefile.c_str());
	if (!file) return def_notfound;
	fsection = trim(ucase(fsection));
        fitem = trim(ucase(fitem));

	line = "";
	while ((section(line) != fsection) && (!file.eof()))
              {
              file.getline(c, 256);
              line = c;
	      line = trim(line);
              }
	if (file.eof())
	   {
	   file.close();
	   return def_notfound;
	   }

	while ((item(line) != fitem) && !file.eof())
              {
              file.getline(c, 256);
              line = c;
	      line = trim(line);
              }
	file.close();
	if ((file.eof()) && (item(line) != fitem)) return def_notfound;
	line = ucase(param(line));
        if ((line == "TRUE") || (line == "YES") || (line == "SI") || (line == "ON"))
           return true;
	   else
	   return false;
}

bool replacestring(string FSection, string FItem, string NewS)
{
	FILE *Appoggio, *RF;
        char c[256];
	string Line;

        if (readstring(FSection, FItem, "") == "") return false;
        Appoggio = fopen("./$INITMP$.$$$", "w");
	if (!Appoggio) return false;
	RF = fopen(resourcefile.c_str(), "r");
        FSection = trim(ucase(FSection));
        FItem = trim(ucase(FItem));
        do
		{
                fgets(c, 256, RF);
		Line = c;
                fputs(c, Appoggio);
		}
                while (section(Line) == FSection);
        do
		{
                fgets(c, 256, RF);
		Line = c;
                if (item(Line) == FItem)
			{
			string s = item(Line)+" = "+NewS;
			s += 0x00D; s += +0x00A;
                  	fputs(s.c_str(), Appoggio);
			}
                  	else
			fputs(c, Appoggio);
		}
                while (item(Line) != FItem);
        do
		{
                fgets(c, 256, RF);
                if (!feof(RF)) fputs(c, Appoggio);
		}
                while (!feof(RF));
        fclose(RF);
        fclose(Appoggio);
	unlink(resourcefile.c_str());
        rename("./$INITMP$.$$$", resourcefile.c_str());
	return true;
}

string get_selfpath(){
    char buff[1024];
    
    #ifdef WIN32
        char *cutoff = NULL;
        
        GetModuleFileName(NULL, buff, MAX_PATH);
        cutoff = strrchr(buff, '\\');
        *(cutoff+1) = '\0';      //get rid of the +1 if you don't want the trailing \ character
        
        return std::string(buff);
    #else
		resourcefile = "";
		ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
	
		if (len != -1){
                    char *cutoff = NULL;
                    
                    cutoff = strrchr(buff, '/');
                    *(cutoff+1) = '\0';      //get rid of the +1 if you don't want the trailing \ character
		
	}else{
		buff[0] = '\0';
	}
	
	return std::string(buff);
    #endif
}

bool init(const char* filename){
	string selfpath = get_selfpath();
	
	if (selfpath.length() > 0){
		resourcefile = selfpath + filename;
		if (access(resourcefile.c_str(), F_OK ) != -1)
			return true;
	}
	
	//printf("file not found: %s %s \r\n", selfpath, resourcefile.c_str());
	return false;
}

}

extern "C" {
void zini_readstring(char* outstr, const char* fsection, const char* fitem, const char* def_notfound){
	string s = zini::readstring(fsection, fitem, def_notfound);
	strcpy(outstr, s.c_str());
}

int zini_readinteger(const char* fsection, const char* fitem, int def_notfound){
	return zini::readinteger(fsection, fitem, def_notfound);
}

bool zini_readboolean(const char* fsection, const char* fitem, bool def_notfound){
	return zini::readboolean(fsection, fitem, def_notfound);
}

int zini_readninteger(const char* fsection, const char* fitem, unsigned n, int def_notfound){
	return zini::readninteger(fsection, fitem, n, def_notfound);
}

bool zini_replacestring(const char* FSection, const char* FItem, const char* NewS){
	return zini::replacestring(FSection, FItem, NewS);
}

bool zini_init(const char* filename){
	return zini::init(filename);
}

void get_exepath(char* path){
	string selfpath = zini::get_selfpath();
	strcpy(path, selfpath.c_str());
}


}