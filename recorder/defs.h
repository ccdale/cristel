/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * defs.h
 *
 * Started: Sunday 23 December 2012, 19:43:27
 * Last Modified: Friday 25 July 2014, 09:12:19
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

/*
#define CCA_DEFAULT_DAEMONIZE "1"
#define CCA_DEFAULT_CONFIGFILE "/etc/qillsd.conf"
#define CCA_DEFAULT_SUMMARY_SIZE "500"
#define CCA_DEFAULT_BUFFERSIZE "2048000"
#define CCA_DEFAULT_SLEEPTIME "1"
#define CCA_DEFAULT_SUMMARIZE "0"
#define CCA_DEFAULT_FPOS "0"
#define CCA_DEFAULT_SEARCH "(ERROR:.*)"
#define CCA_DEFAULT_TIME_FORMAT "%Y-%m-%d %H:%M:%S"
#define CCA_DEFAULT_FILEBIN "/usr/bin/file"
#define CCA_DEFAULT_FILEBINARGS "-ib"
#define CCA_DEFAULT_GUNZIPBIN "/bin/gunzip"
#define CCA_DEFAULT_GUNZIPBINARGS "-c"
#define CCA_DEFAULT_UNZIPBIN "/usr/bin/unzip"
#define CCA_DEFAULT_UNZIPBINARGS "-p"
#define CCA_DEFAULT_INDEXROTATEDLOGFILES "1"
#define CCA_DEFAULT_RLFEXT ""
#define CCA_DEFAULT_MISSINGFILES "0"
#define CCA_DEFAULT_DBHOST "localhost"
#define CCA_DEFAULT_DBDB "qills"
#define CCA_DEFAULT_DBTABLE_SERVER "lognames"
#define CCA_DEFAULT_DBTABLE_LOG "loglines"
#define CCA_DEFAULT_DBUSER "logserver"
#define CCA_DEFAULT_DBPASS "logserver"
#define CCA_DEFAULT_DBENV "live"
#define CCA_DEFAULT_DBSERVICE "qills"
#define CCA_DEFAULT_DBTYPE "app"
#define CCA_DEFAULT_DBNUMBER "1"

#define CCA_HOME "/"
#define CCA_LOCK_FILE "/var/run/qillsd.pid"
#define MAX_MSG_LEN 4096
#define CCA_MAX_ROTATED_FILES 25
*/
