/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * cristel.h
 * 
 * Started: Thursday 24 July 2014, 13:07:18
 * Last Modified: Thursday  6 October 2016, 12:48:16
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

#include <sys/types.h>
#include "defs.h"
#include <locale.h>
#include "macros.h"
#include "tools.h"
#include "configfile.h"
#include <argtable2.h>
#include "dvbctrl.h"
#include "dvbcmds.h"

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

/* globals */
struct ConfigItem *configuration;
struct Program *currentprogram;
struct Single *single;
int llevel=6;
int timetodie=0;
int reload=0;

char *argprocessing(int argc,char **argv);
void catchsignal(int sig);
int countFutureRecordings(sqlite3 *db);
void daemonize(char *conffile);
int fillProgram(void *NotUsed, int argc, char **argv, char **ColName);
void freeProgram(void);
int getNextToRecord(sqlite3 *db);
void initProgram(void);
void logProgram(void);
void logSingle(void);
int returnSingle(void *unused, int argc, char **argv, char **colname);
void setDefaultConfig( void );
void startDvbStreamer(int adaptor);
void stopDvbStreamer(int adaptor);
int sqlexec(sqlite3 *db, char *sql, void *callback);
void mainLoop();
int main(int argc,char **argv);
