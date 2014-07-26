/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * tools.h
 *
 * Started: Wednesday 21 November 2012, 12:44:26
 * Last Modified: Friday 25 July 2014, 08:56:52
 *
 * Copyright (c) 2014 Chris Allison chris.allison@hotmail.com
 *
 * This file is part of cristel.
 * 
 * cristel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cristel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with cristel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "defs.h"
#include "macros.h"

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
