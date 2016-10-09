/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbctrl.h
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Sunday 27 July 2014, 06:09:19
 * Last Modified: Saturday  8 October 2016, 09:01:23
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
#ifndef DVBCTRL_H
#define DVBCTRL_H

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
struct ServiceInfo {
    int type;
    int ca;
    int mux;
    int pmtpid;
    char *name;
    char *source;
    char *ID;
};
struct ColonParse {
    char *key;
    char *val;
    char *tmp;
};
struct FilterStatus {
    int num;
    char *name;
    char *channel;
    char *mrl;
};
struct AdaptorStatus {
    int adaptornum;
    int numfilters;
    int recording;
    int mux;
    struct FilterStatus **FS;
};
#endif

void setupConnect(int adapternum);
void closeConnect();
void prepare_streamer_data(void);
void free_streamer_data(void);
int dvbc_connect(int adapternum);
int Authenticate(char *username, char *password);
int sendData(char *cmd);
int request(char *cmd);
int rcvData(void);
void addLineToBuffer();
struct ServiceInfo *newServiceInfo(void);
void freeServiceInfo(struct ServiceInfo *SI);
struct ServiceInfo *serviceInfoParse(char *si);
struct ColonParse *parseColon(char *line);
void updateServiceInfo(struct ServiceInfo *SI,char *key,char *val);
struct ServiceInfo *getServiceInfo(char *service);
char * dvbcommand(char *cmd,int adaptornum);
int dvbcmd(char *cmd,int adaptornum,char *output);
