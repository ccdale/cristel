/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * sql.c
 *
 * Last Modified: Sunday 16 October 2016, 11:58:26
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

#include "sql.h"
int countCurrentRecordings(sqlite3 *db)/* {{{1 */
{
    char *sql;
    int rc=0;
    time_t now;
    int numr=0;

    resetSingle();
    now=time(NULL);
    sql=fitstring("select count(*) as xcount from recording where end > %ld",now);
    rc=sqlexec(db,sql,returnSingle);
    free(sql);
    if(rc==0){
        numr=atoi(single->val);
    }
    if(numr==0){
        INFO("Not currently recording");
    }else if(numr==1){
        INFO("Currently recording 1 program");
    }else{
        INFO("Currently recording %d programs.",numr);
    }
    return numr;
}/* }}} */
int countFutureRecordings(sqlite3 *db)/* {{{1 */
{
    char *sql;
    int rc=0;
    time_t now;
    int numr=0;

    resetSingle();
    now=time(NULL);
    sql=fitstring("select count(*) as xcount from schedule where record='y' and end > %ld",now+30);
    rc=sqlexec(db,sql,returnSingle);
    free(sql);
    if(rc==0){
        numr=atoi(single->val);
    }
    INFO("number of future recordings: %d",numr);
    return numr;
}/* }}} */
int fillProgram(void *NotUsed, int argc, char **argv, char **ColName)/*{{{*/
{
    int x;
    /* to clear up the unused var warning */
    if(NotUsed){
    }
    for(x=0;x<argc;x++){
        if((strcmp(ColName[x],"id")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->id=atoi(argv[x]);
            /* DEBUG("Check: %d", currentprogram->id);*/
        }else if((strcmp(ColName[x],"event")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->event=atoi(argv[x]);
            /* DEBUG("Check: %d", currentprogram->event);*/
        }else if((strcmp(ColName[x],"muxid")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->muxid=atoi(argv[x]);
            /* DEBUG("Check: %d", currentprogram->muxid);*/
        }else if((strcmp(ColName[x],"start")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->start=atoi(argv[x]);
            /* DEBUG("Check: %d", currentprogram->start);*/
        }else if((strcmp(ColName[x],"end")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->end=atoi(argv[x]);
            /* DEBUG("Check: %d", currentprogram->end);*/
        }else if((strcmp(ColName[x],"adaptor")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->adaptor=atoi(argv[x]);
            /* DEBUG("Check: %d", currentprogram->adaptor);*/
        }else if((strcmp(ColName[x],"source")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->source=strdup(argv[x]);
            /* DEBUG("Check: %s", currentprogram->source);*/
        }else if((strcmp(ColName[x],"cname")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->cname=strdup(argv[x]);
            /* DEBUG("Check: %s", currentprogram->cname);*/
        }else if((strcmp(ColName[x],"title")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->title=strdup(argv[x]);
            /* DEBUG("Check: %s", currentprogram->title);*/
        }else if((strcmp(ColName[x],"description")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->description=strdup(argv[x]);
            /* DEBUG("Check: %s", currentprogram->description);*/
        }else if((strcmp(ColName[x],"progid")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->progid=strdup(argv[x]);
            /* DEBUG("Check: %s", currentprogram->progid);*/
        }else if((strcmp(ColName[x],"seriesid")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->seriesid=strdup(argv[x]);
            /* DEBUG("Check: %s", currentprogram->seriesid);*/
        }else if((strcmp(ColName[x],"record")==0) && (argv[x]!=NULL)){
            /* DEBUG("Setting %s to %s",ColName[x],argv[x]);*/
            currentprogram->record=strdup(argv[x]);
            /* DEBUG("Check: %s", currentprogram->record);*/
        }else{
            if(argv[x]==NULL){
                DEBUG("Column %s is NULL",ColName[x]);
            }else{
                WARN("unexpected Column name '%s' in result",ColName[x]);
            }
        }
    }
    logProgram();
    return 0;
}/*}}}*/
void freeProgram(void)/* {{{1 */
{
    if(currentprogram){
        if(currentprogram->source){
            free(currentprogram->source);
        }
        if(currentprogram->cname){
            free(currentprogram->cname);
        }
        if(currentprogram->title){
            free(currentprogram->title);
        }
        if(currentprogram->description){
            free(currentprogram->description);
        }
        if(currentprogram->progid){
            free(currentprogram->progid);
        }
        if(currentprogram->seriesid){
            free(currentprogram->seriesid);
        }
        if(currentprogram->record){
            free(currentprogram->record);
        }
        free(currentprogram);
    }
    if(single){
        if(single->colname){
            free(single->colname);
        }
        if(single->val){
            free(single->val);
        }
        free(single);
    }
}/* }}} */
int getNextToEnd(sqlite3 *db)/* {{{1 */
{
    char *sql;
    int rc=0;
    int numr;

    resetProgram();
    numr=countCurrentRecordings(db);
    if(numr>0){
        sql=fitstring("select * from recording order by end asc limit 1");
        rc=sqlexec(db,sql,fillProgram);
        free(sql);
    }
    return rc;
}/* }}} */
int getNextToRecord(sqlite3 *db)/* {{{1 */
{
    char *sql;
    int rc=0;
    time_t now;
    int numr;

    resetProgram();
    numr=countFutureRecordings(db);
    if(numr>0){
        now=time(NULL);
        sql=fitstring("select * from schedule where record='y' and end > %ld and start <= %ld order by start asc limit 1",now,now+30);
        /*sql=fitstring("select * from schedule where record='y' and start > %ld order by start asc limit 1",now);*/
        rc=sqlexec(db,sql,fillProgram);
        free(sql);
    }
    return rc;
}/* }}} */
int getImminentRecord(sqlite3 db)/* {{{1 */
{
    char *sql;
    int rc=0;
    time_t now;
    int numr;

    resetProgram();
    numr=countFutureRecordings(db);
    if(numr>0){
        now=time(NULL);
        /* sql=fitstring("select * from schedule where record='y' and end > %ld and start < %ld",now,now-30); */
        sql=fitstring("select * from schedule where record='y' and start > %ld order by start asc limit 1",now);
        rc=sqlexec(db,sql,fillProgram);
        free(sql);
    }
    return rc;
}/* }}} */
void initProgram(void)/* {{{1 */
{
    currentprogram=xcalloc(sizeof(struct Program),1);
    resetProgram();
    single=xcalloc(sizeof(struct Single),1);
    resetSingle();
}/* }}} */
void logSingle(void)/*{{{*/
{
    if(single){
        if(single->colname){
            DEBUG("  S: colname: %s",single->colname);
        }
        if(single->val){
            DEBUG("  S: val: %s",single->val);
        }
    }
}/*}}}*/
void logProgram(void)/*{{{*/
{
    if(currentprogram){
        if(currentprogram->id){
            DEBUG("  P: id: %d",currentprogram->id);
        }else{
            DEBUG("  P: id: not set");
        }
        if(currentprogram->event){
            DEBUG("  P: event: %d",currentprogram->event);
        }else{
            DEBUG("  P: event: not set");
        }
        if(currentprogram->muxid){
            DEBUG("  P: muxid: %d",currentprogram->muxid);
        }else{
            DEBUG("  P: muxid: not set");
        }
        if(currentprogram->start){
            DEBUG("  P: start: %d",currentprogram->start);
        }else{
            DEBUG("  P: start: not set");
        }
        if(currentprogram->end){
            DEBUG("  P: end: %d",currentprogram->end);
        }else{
            DEBUG("  P: end: not set");
        }
        if(currentprogram->adaptor){
            DEBUG("  P: adaptor: %d",currentprogram->adaptor);
        }else{
            DEBUG("  P: adaptor: not set");
        }
        if(currentprogram->title){
            DEBUG("  P: title: %s",currentprogram->title);
        }else{
            DEBUG("  P: title: not set");
        }
        if(currentprogram->source){
            DEBUG("  P: source: %s",currentprogram->source);
        }else{
            DEBUG("  P: source: not set");
        }
        if(currentprogram->cname){
            DEBUG("  P: cname: %s",currentprogram->cname);
        }else{
            DEBUG("  P: cname: not set");
        }
        if(currentprogram->description){
            DEBUG("  P: description: %s",currentprogram->description);
        }else{
            DEBUG("  P: description: not set");
        }
        if(currentprogram->progid){
            DEBUG("  P: progid: %s",currentprogram->progid);
        }else{
            DEBUG("  P: progid: not set");
        }
        if(currentprogram->seriesid){
            DEBUG("  P: seriesid: %s",currentprogram->seriesid);
        }else{
            DEBUG("  P: seriesid: not set");
        }
        if(currentprogram->record){
            DEBUG("  P: record: %s",currentprogram->record);
        }else{
            DEBUG("  P: record: not set");
        }
    }else{
        DEBUG("current program not setup");
    }
}/*}}}*/
void resetProgram(void)/* {{{1 */
{
    currentprogram->id=0;
    currentprogram->event=0;
    currentprogram->muxid=0;
    currentprogram->start=0;
    currentprogram->end=0;
    currentprogram->adaptor=0;
    currentprogram->filter=0;
    currentprogram->fnfz=0;
    if(currentprogram->source){
        free(currentprogram->source);
    }
    currentprogram->source=NULL;
    if(currentprogram->cname){
        free(currentprogram->cname);
    }
    currentprogram->cname=NULL;
    if(currentprogram->title){
        free(currentprogram->title);
    }
    currentprogram->title=NULL;
    if(currentprogram->description){
        free(currentprogram->description);
    }
    currentprogram->description=NULL;
    if(currentprogram->progid){
        free(currentprogram->progid);
    }
    currentprogram->progid=NULL;
    if(currentprogram->seriesid){
        free(currentprogram->seriesid);
    }
    currentprogram->seriesid=NULL;
    if(currentprogram->record){
        free(currentprogram->record);
    }
    currentprogram->record=NULL;
    if(currentprogram->fn){
        free(currentprogram->fn);
    }
    currentprogram->fn=NULL;
}/* }}} */
void resetSingle(void)/* {{{1 */
{
    if(single->colname){
        free(single->colname);
    }
    single->colname=NULL;
    if(single->val){
        free(single->val);
    }
    single->val=NULL;
}/* }}} */
int returnSingle(void *unused, int argc, char **argv, char **colname)/* {{{1 */
{
    /* removing compiler warning about *unused being 'unused' */
    if(unused){
    }
    if(argc==1){
        single->colname=strdup(colname[0]);
        single->val=strdup(argv[0]);
    }else{
        WARN("returnSingle: invalid number of arguments: %d",argc);
    }
    logSingle();
    return 0;
}/* }}} */
int sqlexec(sqlite3 *db, char *sql, void *callback)/* {{{1 */
{
    char *sqlerr=0;
    int rc=0;

    DEBUG("Executing sql: %s",sql);
    rc=sqlite3_exec(db,sql,callback,0,&sqlerr);
    if(rc!=SQLITE_OK){
        WARN("error executing sql: %s, error code: %d, errmsg: %s",sql,rc,sqlerr);
        sqlite3_free(sqlerr);
        return rc;
    }
    return 0;
}/* }}} */
void updateRecordProgram(sqlite3 *db,char *status)/* {{{1 */
{
    char *sql;

    sql=fitstring("update schedule set record='%s' where id=%d",status,currentprogram->id);
    sqlexec(db,sql,NULL);
    free(sql);
    sql=fitstring("insert into recording (source,cname,event,muxid,start,end,title,description,progid,seriesid,adaptor,filepath) values ('%s','%s',%d,%d,%d,%d,'%s','%s','%s','%s',%d,'%s')",currentprogram->source,currentprogram->cname,currentprogram->event,currentprogram->muxid,currentprogram->start,currentprogram->end,currentprogram->title,currentprogram->description,currentprogram->progid,currentprogram->seriesid,currentprogram->adaptor,currentprogram->fn);
    sqlexec(db,sql,NULL);
    free(sql);
}/* }}} */
void updateRecorded(sqlite3 *db)/* {{{1 */
{
    char *sql;

    sql=fitstring("insert into recorded (source,cname,event,muxid,start,end,title,description,progid,seriesid,adaptor,filepath) values ('%s','%s',%d,%d,%d,%d,'%s','%s','%s','%s',%d,'%s')",currentprogram->source,currentprogram->cname,currentprogram->event,currentprogram->muxid,currentprogram->start,currentprogram->end,currentprogram->title,currentprogram->description,currentprogram->progid,currentprogram->seriesid,currentprogram->adaptor,currentprogram->fn);
    sqlexec(db,sql,NULL);
    free(sql);
    sql=fitstring("delete from recording where id = %d",currentprogram->id);
    sqlexec(db,sql,NULL);
    free(sql);
}/* }}} */
