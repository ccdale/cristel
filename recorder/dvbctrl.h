/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbctrl.h
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Sunday 27 July 2014, 06:09:19
 * Last Modified: Friday  8 January 2016, 04:35:49
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
#include "configfile.h"
#include "tools.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

struct StreamerData {
    FILE *socketfp;
    int connected;
    int authenticated;
    int errornumber;
    char *ver;
    char *errmsg;
    char *line;
    char *data;
};

static void setupConnect(int adapternum);
static void closeConnect(int adapternum);
static void prepare_streamer_data(int adapternum);
static void free_streamer_data(int adapternum);
static int dvbc_connect(int adapternum);
static int Authenticate(int adaptornum, char *username, char *password);
static int sendData(int adaptornum, char *cmd);
static int request(int adaptornum, char *cmd);
static int rcvData(int adaptornum);
static void addLineToBuffer(int adaptornum);
char * dvbcmd(int adaptornum, char *cmd);
char * lsservices(int adaptornum);
