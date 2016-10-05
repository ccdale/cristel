/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * defs.h
 *
 * Started: Sunday 23 December 2012, 19:43:27
 * Last Modified: Wednesday  5 October 2016, 08:49:18
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

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sqlite3.h>

#define PROGNAME "cristel"
#define PROGVERSION "0.01"
#define PROGEMAIL "chris.allison@hotmail.com"
#define PROGCOPYRIGHT "Copyright (C) 2014 Chris Allison"
#define PROGLICENSE "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n"
#define PROGCONF "/home/chris/src/cristel/recorder/"PROGNAME".conf"

#define CCA_HOME "/"
#define CCA_LOCK_FILE "/home/chris/"PROGNAME".pid"
#define MAX_MSG_LEN 4096

#define CCA_DEFAULT_USERNAME "chris"
#define CCA_DEFAULT_NUMADAPTORS "2"
#define CCA_DEFAULT_DVBHOST "127.0.0.1"
#define CCA_DEFAULT_DVBUSER "tvc"
#define CCA_DEFAULT_DVBPASS "tvc"
#define CCA_DEFAULT_DVBBANNER "DVBStreamer/"
#define CCA_DEFAULT_DBPATH "/home/chris/.epgdb"
#define CCA_DEFAULT_DBNAME "cristel.db"
#define CCA_DEFAULT_RECPATH "/home/chris/Videos/tv"
#define CCA_DEFAULT_CONFIGFILE PROGCONF

#define REMOTEINTERFACE_PORT 54197

#define MAX_LINE_LENGTH 256
#define RCV_BUFFER_LENGTH 8192

