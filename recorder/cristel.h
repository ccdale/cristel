/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * cristel.h
 * 
 * Started: Thursday 24 July 2014, 13:07:18
 * Last Modified: Wednesday 31 December 2014, 10:22:08
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
#include "macros.h"
/* #include "globals.h" */
#include "tools.h"
#include "configfile.h"
#include <argtable2.h>
/* mysql connection */
#include <my_global.h>
#include <mysql.h>

/* globals */
struct ConfigItem *configuration;
struct sigaction *siga;
int timetodie=0;
MYSQL *myconn;

void catchsignal(int sig);
int daemonize();
int setDefaultConfig( void );
void startDvbStreamer(int adaptor);
void stopDvbStreamer(int adaptor);
int mainLoop();
int main(int argc,char **argv);
