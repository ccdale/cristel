/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * cristel.c
 *
 * Started: Thursday 24 July 2014, 13:05:39
 * Last Modified: Thursday  6 October 2016, 13:15:13
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

#include "cristel.h"

char *argprocessing(int argc,char **argv)/* {{{ */
{
    char *conffile=NULL;
    /* Define the allowable command line options, collecting them in argtable[] */
    struct arg_file *conf = arg_file0("c","conf-file",PROGCONF,"configuration file");
    struct arg_lit *help = arg_lit0("h","help","print this help and exit");
    struct arg_int *loglevel = arg_int0("l","log-level","<n>","7=LOG-DEBUG .. 0=LOG_EMERG - default: 6 (LOG_INFO)");
    struct arg_lit *vers = arg_lit0("v","version","print version information and exit");
    struct arg_end *end  = arg_end(20);

    void *argtable[] = {conf,help,loglevel,vers,end};

    int nerrors;

    /* check the argtable[] entries were correctly allocated */
    if(arg_nullcheck(argtable) == 0){
        /* set default config filename */
        conf->filename[0]=PROGCONF;
        /* parse the command line */
        nerrors = arg_parse(argc,argv,argtable);

        /* check for help before error processing */
        if(help->count > 0){
            /* print the help/usage statement and exit */
            printf("Usage: %s",PROGNAME);
            arg_print_syntax(stdout,argtable,"\n");
            printf("%s is a daemon to manage tv recordings from the dvb adaptor(s).\n\n",PROGNAME);
            arg_print_glossary(stdout,argtable,"  %-20s %s\n");
            /* free up memory used for argument processing */
            arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
            printf("\nreport bugs to %s\n",PROGEMAIL);
            return 0;
        }
        /* check for version request before error checking */
        if(vers->count > 0){
            /* free up memory used for argument processing */
            arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
            /* print the requested information and exit */
            printf("%s %s\n",PROGNAME,PROGVERSION);
            printf("%s\n%s",PROGCOPYRIGHT,PROGLICENSE);
            return 0;
        }
        /* check for errors in command line arguments */
        if(nerrors > 0){
            /* display the errors in the arg_end struct */
            arg_print_errors(stdout,end,PROGNAME);
            printf("Try '%s --help' for more information.\n",PROGNAME);
            /* free up memory used for argument processing */
            arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
            CCAE(1,"Failed to process command line correctly");
        }
        if(conf->count > 0){
            conffile=strdup(conf->filename[0]);
        }
        if(loglevel->count > 0){
            llevel=loglevel->ival[0];
        }
    }
    /* free up memory used for argument processing */
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    /* command line processing completed */
    return conffile;
}/* }}} */
void catchsignal(int sig)/* {{{1 */
{
    DBG("in sig handler");
    switch(sig){
        case SIGCHLD:
            DBG("SIGCHLD signal caught");
            int childStatus;
            pid_t returnValue = waitpid(-1, &childStatus, 0);
            if (returnValue > 0)
            {
                if (WIFEXITED(childStatus)){
                    WARN("Child Exit Code: %d\n", WEXITSTATUS(childStatus));
                }else{
                    INFO("Child exit Status: 0x%.4X\n", childStatus);
                }
            }else if (returnValue == 0){
                INFO("Child process still running\n");
            }else{
                WARN("error: %d: %s",errno,strerror(errno));
            }
            break;
        case SIGTERM:
            DBG("SIGTERM signal caught");
            timetodie=1;
            break;
        case SIGUSR1:
            DBG("SIGUSR1 signal caught");
            reload=1;
            break;
        case SIGALRM:
            DBG("SIGALRM signal caught");
            break;
    }
} /* }}} */
int countFutureRecordings(sqlite3 *db)/* {{{1 */
{
    char *sql;
    int rc=0;
    time_t now;
    int numr=0;

    now=time(NULL);
    sql=fitstring("select count(*) as xcount from schedule where record='y' and start > %ld",now);
    rc=sqlexec(db,sql,returnSingle);
    free(sql);
    if(rc==0){
        numr=atoi(single->val);
    }
    INFO("number of future recordings: %d",numr);
    return numr;
}/* }}} */
void daemonize(char *conffile)/* {{{1 */
{
    int i,lfp;
    char str[MAX_MSG_LEN];
    int junk,ret;
    struct passwd *pwd;
    char *username;
    struct sigaction siga;

    if(getppid()==1) return; /* already a daemon */
    DBG("Forking");
    i=fork();
    if (i<0) 
    {
        CCAE(1,"fork failed. Exiting");
    }
    if (i>0) 
    {
        DBG("parent exiting after fork");
        exit(0);
    } /* parent exits */
    /* child (daemon) continues */
    DBG("fork success. Child process continues.");
    setsid(); /* obtain a new process group */
    DBG("closing file descriptors");
    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    DBG("redirecting standard i/o to /dev/null");
    i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standard I/O */
    DBG("setting umask to 027");
    umask(027); /* set newly created file permissions */

    if((ret=initConfig())==0){
        DBG("setting default config");
        setDefaultConfig();
        if(conffile){
            updateConfig("configfile",conffile);
        }
        DBGL("Getting config from %s",configValue("configfile"));
        getConfigFromFile( configValue("configfile") );
    }else{
        CCAE(1,"Cannot initialise configuration.");
    }
    DBG("Dropping privileges");
    username=configValue("username");
    if((pwd=getpwnam(username))==NULL || strlen(username)==0){
        CCAE(1,"user %s not found, will not run as root, exiting",username);
    }
    errno=0;
    junk=setgid(pwd->pw_gid);
    if(errno){
        CCAE(1,"failed to drop group privileges (grpid: %d), will not run as root, exiting",pwd->pw_gid);
    }
    errno=0;
    junk=setuid(pwd->pw_uid);
    if(errno){
        CCAE(1,"failed to drop privileges, will not run as root, exiting");
    }

    DBGL("cd'ing to %s",pwd->pw_dir);
    chdir(pwd->pw_dir); /* change running directory */
    if ((junk=filesize(CCA_LOCK_FILE)!=-1)){
        CCAE(1,"lock file %s exists, exiting",CCA_LOCK_FILE);
    }
    DBG("Creating lock file");
    lfp=open(CCA_LOCK_FILE,O_RDWR|O_CREAT,0640);
    if (lfp<0) {
        CCAE(1,"Failed to create lock file, exiting.");
    } /* can not open */
    if (lockf(lfp,F_TLOCK,0)<0) {
        CCAE(1,"Cannot lock, another instance running? qitting");
    } /* can not lock */
    DBG("File locked.");
    /* first instance continues */
    sprintf(str,"%d",getpid());
    write(lfp,str,strlen(str)); /* record pid to lockfile */
    DBG("pid written to lock file");

    DBG("setting signal handlers");
    siga.sa_flags=0;
    /* block every signal during handler */
    sigfillset(&siga.sa_mask);

    /* ignored signals */
    siga.sa_handler=SIG_IGN;
    if((junk=sigaction(SIGTSTP,&siga,NULL))!=0){
        CCAE(1,"cannot ignore signal SIGTSTP");
    }
    siga.sa_handler=SIG_IGN;
    if((junk=sigaction(SIGTTOU,&siga,NULL))!=0){
        CCAE(1,"cannot ignore signal SIGTTOU");
    }
    siga.sa_handler=SIG_IGN;
    if((junk=sigaction(SIGTTIN,&siga,NULL))!=0){
        CCAE(1,"cannot ignore signal SIGTTIN");
    }
    siga.sa_handler=SIG_IGN;
    if((junk=sigaction(SIGHUP,&siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGHUP");
    }
    siga.sa_handler=SIG_IGN;
    if((junk=sigaction(SIGINT,&siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGINT");
    }

    /* interesting (caught) signals */
    siga.sa_handler=catchsignal;
    if((junk=sigaction(SIGALRM,&siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGALRM");
    }
    siga.sa_handler=catchsignal;
    if((junk=sigaction(SIGUSR1,&siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGUSR1");
    }
    siga.sa_handler=catchsignal;
    if((junk=sigaction(SIGCHLD,&siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGCHLD");
    }
    siga.sa_handler=catchsignal;
    if((junk=sigaction(SIGTERM,&siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGTERM");
    }
    DBG(PROGNAME" daemonized");
}/* }}} */
int fillProgram(void *NotUsed, int argc, char **argv, char **ColName)/*{{{*/
{
    int x;
    for(x=0;x<argc;x++){
        if((strcmp(ColName[x],"id")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->id=atoi(argv[x]);
            DEBUG("Check: %d", currentprogram->id);
        }else if((strcmp(ColName[x],"event")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->event=atoi(argv[x]);
            DEBUG("Check: %d", currentprogram->event);
        }else if((strcmp(ColName[x],"muxid")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->muxid=atoi(argv[x]);
            DEBUG("Check: %d", currentprogram->muxid);
        }else if((strcmp(ColName[x],"start")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->start=atoi(argv[x]);
            DEBUG("Check: %d", currentprogram->start);
        }else if((strcmp(ColName[x],"end")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->end=atoi(argv[x]);
            DEBUG("Check: %d", currentprogram->end);
        }else if((strcmp(ColName[x],"adaptor")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->adaptor=atoi(argv[x]);
            DEBUG("Check: %d", currentprogram->adaptor);
        }else if((strcmp(ColName[x],"source")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->source=strdup(argv[x]);
            DEBUG("Check: %s", currentprogram->source);
        }else if((strcmp(ColName[x],"cname")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->cname=strdup(argv[x]);
            DEBUG("Check: %s", currentprogram->cname);
        }else if((strcmp(ColName[x],"title")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->title=strdup(argv[x]);
            DEBUG("Check: %s", currentprogram->title);
        }else if((strcmp(ColName[x],"description")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->description=strdup(argv[x]);
            DEBUG("Check: %s", currentprogram->description);
        }else if((strcmp(ColName[x],"progid")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->progid=strdup(argv[x]);
            DEBUG("Check: %s", currentprogram->progid);
        }else if((strcmp(ColName[x],"seriesid")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->seriesid=strdup(argv[x]);
            DEBUG("Check: %s", currentprogram->seriesid);
        }else if((strcmp(ColName[x],"record")==0) && (argv[x]!=NULL)){
            DEBUG("Setting %s to %s",ColName[x],argv[x]);
            currentprogram->record=strdup(argv[x]);
            DEBUG("Check: %s", currentprogram->record);
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
int getNextToRecord(sqlite3 *db)/* {{{1 */
{
    char *sql;
    char *sqlerr=0;
    int rc=0;
    time_t now;
    int numr;

    numr=countFutureRecordings(db);
    if(numr>0){
        now=time(NULL);
        sql=fitstring("select * from schedule where record='y' and start > %ld order by start asc limit 1",now);
        rc=sqlexec(db,sql,fillProgram);
        free(sql);
    }
    return rc;
}/* }}} */
void initProgram(void)/* {{{1 */
{
    currentprogram=xmalloc(sizeof(struct Program));
    currentprogram->id=0;
    currentprogram->event=0;
    currentprogram->muxid=0;
    currentprogram->start=0;
    currentprogram->end=0;
    currentprogram->adaptor=0;
    currentprogram->source=NULL;
    currentprogram->cname=NULL;
    currentprogram->title=NULL;
    currentprogram->description=NULL;
    currentprogram->progid=NULL;
    currentprogram->seriesid=NULL;
    currentprogram->record=NULL;
    single=xmalloc(sizeof(struct Single));
    single->colname=NULL;
    single->val=NULL;
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
int returnSingle(void *unused, int argc, char **argv, char **colname)/* {{{1 */
{
    if(argc==1){
        single->colname=strdup(colname[0]);
        single->val=strdup(argv[0]);
    }else{
        WARN("returnSingle: invalid number of arguments: %d",argc);
    }
    logSingle();
    return 0;
}/* }}} */
void startDvbStreamer(int adaptor)/*{{{*/
{
    char cmd[]="/usr/bin/dvbstreamer";
    char *user;
    char *pass;
    char *adap;
    char adap0[]="0";
    char adap1[]="1";
    int pid;
    int len;

    DBG("forking dvbstreamer");
    pid=fork();
    if(pid==0){
        /*
        len=snprintf(adap,0,"%d",adaptor);
        len++;
        if(( adap=malloc(len)) == NULL){
            CCAE(1,"Out of memory.");
        }
        len=snprintf(adap,len,"%d",adaptor);
        */
        /*
         * this kludge is used instead of asking for memory (as commented above)
         * which I have no way of freeing once I execl (and I am not sure if 
         * freeing the memory is actually necessary or not)
         */
        if(adaptor==0){
            adap=adap0;
        }else{
            adap=adap1;
        }
        user=configValue("dvbuser");
        pass=configValue("dvbpass");
        execl(cmd,cmd,"-d","-a",adap,"-u",user,"-p",pass,(char *)0);
    }
}/*}}}*/
void stopDvbStreamer(int adaptor)/*{{{*/
{
    char *pidfile;
    char *env;
    int len;
    int pid=0;

    /*
     * attempts to stop the dvbstreamer process
     */
    env=getenv("HOME");
    len=snprintf(pidfile,0,"%s/.dvbstreamer/dvbstreamer-%d.pid",env,adaptor);
    len++;
    pidfile=xmalloc(len);
    len=snprintf(pidfile,len,"%s/.dvbstreamer/dvbstreamer-%d.pid",env,adaptor);
    DBGL("reading %s for pid",pidfile);
    pid=readPidFile(pidfile);
    if(pid>0){
        DBGL("Sending SIGTERM to dvbstreamer adaptor %d, pid: %d",adaptor,pid);
        kill((pid_t)pid,SIGTERM);
    }
    free(pidfile);
}/*}}}*/
void setDefaultConfig(void)/*{{{*/
{
    char *tk;
    char *tv;

    tk=strdup("configfile");
    tv=strdup(CCA_DEFAULT_CONFIGFILE);
    updateConfig(tk,tv);
    tk=strdup("dvbhost");
    tv=strdup(CCA_DEFAULT_DVBHOST);
    updateConfig(tk,tv);
    tk=strdup("dvbuser");
    tv=strdup(CCA_DEFAULT_DVBUSER);
    updateConfig(tk,tv);
    tk=strdup("dvbpass");
    tv=strdup(CCA_DEFAULT_DVBPASS);
    updateConfig(tk,tv);
    tk=strdup("username");
    tv=strdup(CCA_DEFAULT_USERNAME);
    updateConfig(tk,tv);
    tk=strdup("numadaptors");
    tv=strdup(CCA_DEFAULT_NUMADAPTORS);
    updateConfig(tk,tv);
    tk=strdup("dbpath");
    tv=strdup(CCA_DEFAULT_DBPATH);
    updateConfig(tk,tv);
    tk=strdup("dbname");
    tv=strdup(CCA_DEFAULT_DBNAME);
    updateConfig(tk,tv);
    tk=strdup("recpath");
    tv=strdup(CCA_DEFAULT_RECPATH);
    updateConfig(tk,tv);
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
void mainLoop()/*{{{*/
{
    int cc=0;
    sqlite3 *db;
    int rc=0;
    char *dbname;
    long flen=0;
    char *sql;
    char *szerr=0;
    char *svc;
    struct ServiceInfo *SI;

    sleep(10);
    sql=xmalloc(4096);
    initProgram();
    dbname=concatFileParts(3,configValue("dbpath"),"/",configValue("dbname"));
    flen=filesize(dbname);
    if(flen!=-1){
        DEBUG("opening db: %s",dbname);
        rc=sqlite3_open(dbname, &db);
        if(rc!=SQLITE_OK){
            CCAE(1,"error opening db: %s, error code: %d",dbname,rc);
        }
    }else{
        CCAE(1,"cannot find db: %s",dbname);
    }
    do{
        if(timetodie!=0){
            INFO("Shutting down");
            break;
        }
        DEBUG("Calling new func: getNextToRecord");
        getNextToRecord(db);
        /*
        DEBUG("Setting up sql string");
        cc=sprintf(sql,"%s","select * from schedule where record='y' order by start asc limit 1;");
        DEBUG("sql: %s",sql);
        rc=sqlite3_exec(db,sql,fillProgram,0,&szerr);
        if(rc!=SQLITE_OK){
            WARN("error executing sql: %s, error code: %d, errmsg: %s",sql,rc,szerr);
            sqlite3_free(szerr);
        }
        */
        break;
        /* sleep(1);*/
        if((++cc)>10){
            cc=selectlcn(0,1);
            cc=selectlcn(1,3);
            SI=getServiceInfo("BBC TWO");
            DEBUG("SI: Name: %s",SI->name);
            DEBUG("SI: Type: %d",SI->type);
            DEBUG("SI: ca: %d",SI->ca);
            DEBUG("SI: ID: %s",SI->ID);
            DEBUG("SI: mux: %d",SI->mux);
            DEBUG("SI: Source: %s",SI->source);
            freeServiceInfo(SI);
            int ff=safeToRecord(0,"ITV");
            DEBUG("safe to record free filter on adaptor 0: %d",ff);
            ff=safeToRecord(1,"ITV");
            DEBUG("safe to record free filter on adaptor 1: %d",ff);
            /* break; */
            cc=0; /* only every 10 seconds */
        }
        if(reload!=0){
            reload=0;
            INFO("Re-reading db");
        }
        alarm(500); /* goto sleep for 500 seconds */
        pause();
    }
    while(1);
    DEBUG("Closing db %s",dbname);
    sqlite3_close(db);
    if(dbname){
        free(dbname);
    }
    freeProgram();
    free(sql);
}/*}}}*/
int main(int argc,char **argv)/* {{{ */
{
    int ret=0;
    char *conffile=NULL;
    int na=1;
    int x;

    conffile=argprocessing(argc,argv);

    NOTICE(PROGNAME" starting");

    daemonize(conffile);

    na=atoi(configValue("numadaptors"));
    for(x=0;x<na;x++){
        startDvbStreamer(x);
    }

    mainLoop();

    for(x=0;x<na;x++){
        stopDvbStreamer(x);
    }

    INFO(PROGNAME" closing");
    DBG("freeing config");
    deleteConfig();
    DBG("deleting lock file");
    unlink(CCA_LOCK_FILE);
    NOTICE(PROGNAME" stopped");
    return ret;
}/*}}}*/
