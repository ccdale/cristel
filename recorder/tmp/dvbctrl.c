/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbctrl.c
 *
 * Started: Sunday 27 July 2014, 06:07:48
 * Last Modified: Tuesday 12 August 2014, 05:43:12
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

#include "dvbctrl.h"

void doCommand(char cmd)
{
    int nlines;

    nlines=request(cmd);
    DBGL("%d lines recieved for request %s",nlines,cmd);
}
void lsjunk()/*{{{*/
{
    int nlines;
    char cmd[]="lsjunk";

    nlines=request(cmd);
    DBGL("%d lines recieved for request %s",nlines,cmd);
    DBGL("%s",rcvbuf);
    clear_dvbcmsg();
}/*}}}*/
void lssfs()/*{{{*/
{
    int nlines;

    sprintf(line,"lssfs");
    nlines=request(line);
    DBGL("%d lines recieved for request lssfs",nlines);
    DBGL("%s",rcvbuf);
    clear_dvbcmsg();
}/*}}}*/
