/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * schedule.c
 *
 * Last Modified: Friday  7 October 2016, 17:06:57
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

#include "schedule.h"

void getepg(void)/* {{{1 */
{
    FILE *fp;
    int status;
    char buffer[1024];

    INFO("starting epg update");
    fp=popen("/home/chris/src/cristel/bin/getepg -c","r");
    if(fp!=NULL){
        while(fgets(buffer,1024,fp) !=NULL){
            INFO("%s",buffer);
        }
        status=pclose(fp);
        if(status==-1){
            WARN("%d Error reported by pclose for getepg update: %s",errno,strerror(errno));
        }
    }else{
        WARN("Failed to execute /home/chris/src/cristel/bin/getepg");
    }
    INFO("epg update completed");
    INFO("schedule update starting");
    fp=popen("/home/chris/src/cristel/bin/cristelfilldb.py","r");
    if(fp!=NULL){
        while(fgets(buffer,1024,fp) !=NULL){
            INFO("%s",buffer);
        }
        status=pclose(fp);
        if(status==-1){
            WARN("%d Error reported by pclose for schedule update: %s",errno,strerror(errno));
        }
    }else{
        WARN("Failed to execute /home/chris/src/cristel/bin/cristelfilldb.py");
    }
}/* }}} */
