/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbconnect.h
 *
 * Started: Saturday  9 August 2014, 15:58:39
 * Last Modified: Sunday 10 August 2014, 17:56:36
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

#define MAX_LINE_LENGTH 256
#define RCV_BUFFER_LENGTH 8192

int dvbc_connect(int adapternum);
void dvbc_disconnect(void);
int Authenticate();
int sendData(char *cmd);
int request(char *cmd);
int receiveData(void);
void addLineToBuffer(void);
void emptyBuffer(void);
void clear_dvbcmsg(void);
