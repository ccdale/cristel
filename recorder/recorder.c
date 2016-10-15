/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * recorder.c
 *
 * Last Modified: Saturday 15 October 2016, 11:34:03
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

int checkNextRecordEnd(sqlite3 *db)/* {{{1 */
{
    int then;

    then=nextToEndI(db);
    if(then<1){
        INFO("Stopping current recording of '%s' from '%s'",currentprogram->title,currentprogram->cname);
        endRecording(db);
        then=INT_MAX;
    }
    return then;
}/* }}} */
int checkNextRecordStart(sqlite3 *db)/* {{{1 */
{
    int then,x;

    then=nextToRecordI(db);
    if(then==INT_MAX){
        INFO("Nothing to record");
    }else{
        if(then<1){
            INFO("Preparing to record %s",currentprogram->title);
            x=recordProgram(db);
            DEBUG("recordProgram returned %d",x);
            then=INT_MAX;
        }
    }
    return then;
}/* }}} */
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
int nextToEndI(sqlite3 *db)/* {{{1 */
{
    int togo=INT_MAX;
    struct tm *tim;
    char *howlong;
    time_t then;

    getNextToEnd(db);
    if(currentprogram->end){
        then=currentprogram->end;
        tim=localtime(&then);
        togo=currentprogram->end - time(NULL);
        if(togo>0){
            howlong=hms(togo);
            INFO("Current recording of '%s' will end in %s at %.2d:%.2d",currentprogram->title,howlong,tim->tm_hour,tim->tm_min);
            free(howlong);
        }else{
            INFO("Current recording of '%s' ends NOW.",currentprogram->title);
        }
    }
    return togo;
}/* }}} */
int nextToRecordI(sqlite3 *db)/* {{{1 */
{
    int togo=INT_MAX;
    struct tm *tim;
    char *howlong;
    time_t then;

    getNextToRecord(db);
    if(currentprogram->start){
        then=currentprogram->start;
        tim=localtime(&then);
        togo=currentprogram->start - time(NULL);
        if(togo>0){
            howlong=hms(togo);
            INFO("Next Recording of '%s' from '%s' starts in %s at %.2d:%.2d.",currentprogram->title,currentprogram->cname,howlong,tim->tm_hour,tim->tm_min);
            free(howlong);
        }else{
            INFO("Next Recording of '%s' from '%s' starts NOW.",currentprogram->title,currentprogram->cname);
        }
    }
    return togo;
}/* }}} */
int recordProgram(sqlite3 *db)/* {{{1 */
{
    char *fn=NULL;
    char *tfn=NULL;
    int ret=1;
    char *rstatus;

    tfn=sensibleFilename(currentprogram->title);
    if(tfn){
        fn=concatFileParts(4,configValue("recpath"),"/",tfn,".ts");
        if(fn){
            if((ret=streamNewProgram(fn,currentprogram))==0){
                rstatus=fitstring("r");
                /* rstatus[0]="r";*/
            }else{
                WARN("recordProgram: failed to start new recording on %s to %s",currentprogram->cname,fn);
                rstatus=fitstring("f");
                /* rstatus[0]="f";*/
            }
            updateRecordProgram(db,rstatus);
            free(rstatus);
            free(fn);
        }else{
            WARN("recordProgram: failed to build filename from recpath: %s and %s",configValue("recpath"),tfn);
        }
        free(tfn);
    }else{
        WARN("recordProgram: failed to allocate memory for filename for title: %s",currentprogram->title);
    }
    return ret;
}/* }}} */
int endRecording(sqlite3 *db)/* {{{1 */
{
    int fnum;
    int ret=1;
    int x;

    if(currentprogram->fn){
        fnum=findFilterForFile(currentprogram->adaptor,currentprogram->fn);
        if(fnum>-1){
            if((x=setsfmrl(currentprogram->adaptor,fnum,"null://"))==0){
                INFO("Stopped recording of %s from %s on adaptor %d",currentprogram->title,currentprogram->cname,currentprogram->adaptor);
                ret=0;
            }else{
                WARN("Failed to stop recording of %s from %s on adaptor %d at filter %d",currentprogram->title,currentprogram->cname,currentprogram->adaptor,fnum);
            }
        }
        updateRecorded(db);
    }
    return ret;
}/* }}} */
