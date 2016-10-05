/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * cristel.c
 *
 * Started: Thursday 24 July 2014, 13:05:39
 * Last Modified: Wednesday  5 October 2016, 09:49:33
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

void mainLoop()/*{{{*/
{
    int cc=0;
    sqlite3 *db;
    char *dbname;
    int fnlen=0;
    long flen=0;
    char *svc;
    struct ServiceInfo *SI;

    dbname=concatFileParts(3,configValue("dbpath"),"/",configValue("dbname"));
    DEBUG("db filename: %s", dbname);
    do{
        if(timetodie!=0){
            INFO("Shutting down");
            break;
        }
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
        alarm(5); /* goto sleep for 5 seconds */
        pause();
    }
    while(1);
    if(dbname){
        free(dbname);
    }
}/*}}}*/
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
