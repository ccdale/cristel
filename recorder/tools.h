/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * tools.h
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Wednesday 21 November 2012, 12:44:26
 * Version: 0.00
 * Revision: $Id: tools.h 46 2013-02-15 07:08:18Z chris.charles.allison@gmail.com $
 * Last Modified: Friday 17 July 2015, 08:07:18
 */

#include "defs.h"
#include "macros.h"

void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
int filenumberFromFilename(char *filename);
char *escapestr(char *str);
struct tm *initTm(void);
char *bname(char *fqfilename);
long filesize(char *filename);
char *newstringpointer(char *str);
long strtolong(char *str);
char *numtostr(long long num);
char *trim(char *str);
char *chomp(char *str) ;
char *ltrim(char *str);
char *rtrim(char *str);
char *ltrimsp(char *str);
char *rtrimsp(char *str);
char *ltrimt(char *str);
char *rtrimt(char *str);
char* lefttrim(char *string, char junk);
char *righttrim(char *string, char junk);
int readPidFile(char *fn);
