/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * recorder.c
 *
 * Last Modified: Sunday  9 October 2016, 09:17:45
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

#include "recorder.h"

char *filenameFromTitle(char *title)/* {{{1 */
{
    char *fn;
    char *path;
    char *tmnum;
    struct tm *tim;
    time_t now;
    
    fn=sensibleFilename(title);
    if(fn==NULL){
        WARN("filenameFromTitle failed: couldn't make a sensible filename from title '%s'",title);
    }else{
        now=time(NULL);
        tim=localtime(&now);
        tmnum=fitstring("%.4d%.2d%.2d%.2d%.2d%.2d",tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday,tim->tm_hour,tim->tm_min,tim->tm_sec);
        if(tmnum==NULL){
            free(fn);
            WARN("filenameFromTitle failed to generate a time string");
        }else{
            path=fitstring("%s%s%s%s%s%s",configValue("recpath"),"/",tmnum,"-",fn,".ts");
            free(tmnum);
            free(fn);
            if(path==NULL){
                WARN("filenameFromTitle failed: couldn't concatFileParts");
            }
        }
    }
    return path;
}/* }}} */
int nextToRecordI(sqlite3 *db)/* {{{1 */
{
    int ret=-1;
    int togo;

    getNextToRecord(db);
    if(currentprogram->start){
        togo=currentprogram->start-time(NULL);
    }
    return ret;
}/* }}} */
int recordProgram(void)/* {{{1 */
{
    char *fn;
    int ff;
    int adaptor;
    int c;
    int ret=1;

    if((fn=filenameFromTitle(currentprogram->title))!=NULL){
        c=2;
        for(adaptor=0;adaptor<c;adaptor++){
            if((ff=safeToRecord(0,currentprogram->cname))!=-1){
                break;
            }
        }
        if(ff!=-1){
            c=setsf(adaptor,ff,currentprogram->cname);
            DEBUG("recordProgram: setsf returned %d",c);
        }else{
            WARN("recordProgram failed to find free filter on any apaptor");
        }
    }else{
        WARN("recordProgram failed to make filename from time and title");
    }
    return ret;
}/* }}} */
