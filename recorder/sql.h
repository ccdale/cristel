/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * sql.h
 *
 * Last Modified: Thursday  6 October 2016, 17:59:09
 *
 * Copyright (c) 2016 Chris Allison chris.allison@hotmail.com
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

#ifndef SQL_H
#define SQL_H
#include "defs.h"
#include "macros.h"
#include "tools.h"
#include "configfile.h"

struct Program{
    int id;
    int event;
    int muxid;
    int start;
    int end;
    int adaptor;
    char *source;
    char *cname;
    char *title;
    char *description;
    char *progid;
    char *seriesid;
    char *record;
};
struct Single{
    char *colname;
    char *val;
};
extern struct Program *currentprogram;
extern struct Single *single;
#endif

int countFutureRecordings(sqlite3 *db);
int fillProgram(void *NotUsed, int argc, char **argv, char **ColName);
void freeProgram(void);
int getNextToRecord(sqlite3 *db);
void initProgram(void);
void logProgram(void);
void logSingle(void);
int returnSingle(void *unused, int argc, char **argv, char **colname);
int sqlexec(sqlite3 *db, char *sql, void *callback);
