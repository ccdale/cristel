/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * schedule.c
 *
 * Last Modified: Friday  7 October 2016, 16:29:34
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
    runepgupdate();
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
void runepgupdate(void)/* {{{1 */
{
    int outpipe[2];
    int saverrno;
    int cpid;
    char buffer[1024];
    pid_t pid;
    int status;

    if(pipe(outpipe)<0){
        WARN("runepgupdate: error creating pipe: %s",strerror(errno));
    }
    if((cpid=fork())<0){
        WARN("runepgupdate: failed to fork: %s",strerror(errno));
        close(outpipe[0]);
        close(outpipe[1]);
    }else if(cpid==0){
        close(outpipe[0]);
        if(dup2(outpipe[1],STDOUT_FILENO)<0){
            CCAE(1,"runepgupdate: failed to duplicate STDOUT: %s",strerror(errno));
        }
        execl("/usr/local/bin/getepg","getepg","-c",NULL);
        saverrno=errno;
        close(outpipe[1]);
        CCAE(1,"runepgupdate: error executing getepg: %s",strerror(saverrno));
    }else{
        close(outpipe[1]);
        while(fgets(buffer,1024,outpipe[0]) !=NULL){
            INFO("%s",buffer);
        }
        close(outpipe[0]);
        pid=wait(&status);
    }
}/* }}} */
