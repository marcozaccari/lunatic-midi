#include "utils.h"

/*#ifndef bool
    #define bool int
    #define true 1
    #define false 0
#endif*/

void zini_readstring(char* outstr, const char* fsection, const char* fitem, const char* def_notfound);
int zini_readinteger(const char* fsection, const char* fitem, int def_notfound);
bool zini_readboolean(const char* fsection, const char* fitem, bool def_notfound);
int zini_readninteger(const char* fsection, const char* fitem, unsigned n, int def_notfound);
bool zini_replacestring(const char* FSection, const char* FItem, const char* NewS);
bool zini_init(const char* filename);
void get_exepath(char* path);

