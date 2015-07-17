/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * cristel.c
 *
 * Started: Thursday 24 July 2014, 13:05:39
 * Last Modified: Friday 17 July 2015, 16:57:21
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

void catchsignal(int sig)/* {{{1 */
{
    DBGL("Received signal: %d",sig);
    timetodie=1;
    /* signal(sig,catchsignal); */
} /* }}} */
int daemonize()/* {{{ */
{
    int i,lfp;
    char str[MAX_MSG_LEN];
    char *env;
    int junk;
    int ret=0;

    if(getppid()==1) return 0; /* already a daemon */
    DBG("Forking");
    i=fork();
    if (i<0) 
    {
        CCAE(1,"fork failed. Exiting");
    }
    if (i>0) 
    {
        DBG("parent returning 1");
        /*
        exit(0);
        */
        return 1;
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
    DBGL("cd'ing to %s",CCA_HOME);
    chdir(CCA_HOME); /* change running directory */
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

    /* testing getting env strings */
    if(env=getenv("HOME")){
        DBG("HOME is");
        DBG(env);
    }
    DBG("setting signal handlers");
    /*
    sigemptyset(siga->sa_mask);
    sigaddset(siga->sa_mask,SIGCHLD);
    sigaddset(siga->sa_mask,SIGTSTP);
    sigaddset(siga->sa_mask,SIGTTOU);
    sigaddset(siga->sa_mask,SIGTTIN);
    */
    DBG("sigchld");
    siga->sa_handler=SIG_IGN;
    if((junk=sigaction(SIGCHLD,siga,NULL))!=0){
        CCAE(1,"cannot ignore signal SIGCHLD");
    }
    DBG("sigtstp");
    siga->sa_handler=SIG_IGN;
    if((junk=sigaction(SIGTSTP,siga,NULL))!=0){
        CCAE(1,"cannot ignore signal SIGTSTP");
    }
    DBG("sigttou");
    siga->sa_handler=SIG_IGN;
    if((junk=sigaction(SIGTTOU,siga,NULL))!=0){
        CCAE(1,"cannot ignore signal SIGTTOU");
    }
    DBG("sigttin");
    siga->sa_handler=SIG_IGN;
    if((junk=sigaction(SIGTTIN,siga,NULL))!=0){
        CCAE(1,"cannot ignore signal SIGTTIN");
    }
    /*
       siga->sa_handler=catchsignal;
       if((junk=sigaction(SIGHUP,siga,NULL))!=0){
       CCAE(1,"cannot set handler for SIGHUP");
       }
       siga->sa_handler=catchsignal;
       if((junk=sigaction(SIGTERM,siga,NULL))!=0){
       CCAE(1,"cannot set handler for SIGHUP");
       }
       */
    DBG("sighup");
    siga->sa_handler=catchsignal;
    if((junk=sigaction(SIGHUP,siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGHUP");
    }
    DBG("sigterm");
    siga->sa_handler=catchsignal;
    if((junk=sigaction(SIGTERM,siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGTERM");
    }
    DBG("sigint");
    siga->sa_handler=catchsignal;
    if((junk=sigaction(SIGINT,siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGINT");
    }
    DBG("sigusr1");
    siga->sa_handler=catchsignal;
    if((junk=sigaction(SIGUSR1,siga,NULL))!=0){
        CCAE(1,"cannot set handler for SIGUSR1");
    }
    DBG(PROGNAME" daemonized");
    DBG("Child returning 0");
    return 0;
}/* }}} */
int setDefaultConfig( void )/* {{{ */
{
    char *tk;
    char *tv;
    int ret;

    if((ret = initConfig()) == 0){
        tk=strdup("dbname");
        tv=strdup(CCA_DEFAULT_DBNAME);
        updateConfig(tk,tv);
        tk=strdup("dbhost");
        tv=strdup(CCA_DEFAULT_DBHOST);
        updateConfig(tk,tv);
        tk=strdup("dbuser");
        tv=strdup(CCA_DEFAULT_DBUSER);
        updateConfig(tk,tv);
        tk=strdup("dbpass");
        tv=strdup(CCA_DEFAULT_DBPASS);
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
    }
    return ret;
}/*}}}*/
int mainLoop()/*{{{*/
{
    int ret=0;
    int cc=0;
    char *svc;

    /*
    dvbc_connect(0);
    */
    do{
        if(timetodie!=0){
            break;
        }
        sleep(1);
        if((++cc)>30){
            svc=lsservices(0);
            DBGL("%s",svc);
            free(svc);
            break;
        }
    }
    while(1);
    return ret;
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
    if(pidfile=malloc(len)){
        len=snprintf(pidfile,len,"%s/.dvbstreamer/dvbstreamer-%d.pid",env,adaptor);
        DBGL("reading %s for pid",pidfile);
        pid=readPidFile(pidfile);
        if(pid>0){
            DBGL("Sending SIGTERM to dvbstreamer adaptor %d, pid: %d",adaptor,pid);
            kill((pid_t)pid,SIGTERM);
        }
        free(pidfile);
    }else{
        CCAE(1,"Out of memory allocating strings for stop dvbstreamer");
    }
}/*}}}*/
int main(int argc,char **argv)/*{{{*/
{
    char *conffile;
    int ret=0;
    int pid;

    setlocale(LC_ALL, "");

    /* Define the allowable command line options, collecting them in argtable[] */
    struct arg_file *conf = arg_file0("c",NULL,"/etc/cristel.conf","configuration file");
    struct arg_lit *help = arg_lit0("h","help","print this help and exit");
    struct arg_lit *vers = arg_lit0("v","version","print version information and exit");
    struct arg_end *end  = arg_end(20);
    void *argtable[] = {conf,help,vers,end};
    int exitcode=0;
    int nerrors;

    /* check the argtable[] entries where correctly allocated */
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
            printf("Start the %s tv recorder\n\n",PROGNAME);
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
            return 1;
        }

        /* command line processing completed */

        conffile=strdup(conf->filename[0]);
        /* free up memory used for argument processing */
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
        
        /* setup daemon */
        if((siga=malloc(sizeof(struct sigaction)))==NULL){
            CCAE(1,"out of memory setting up signal handler.");
        }
        siga->sa_handler=catchsignal;
        sigemptyset(&siga->sa_mask);
        siga->sa_flags=0;
        ret=daemonize();
        if(ret==1){
            DBG("I am the parent, freeing memory and buggering off");
            free(conffile);
            free(siga);
            exit(0);
        }
        DBG("daemonize returned");

        /* setup config */
        if((exitcode=setDefaultConfig()) == 0){
            /*
             * config setup ok
             * read in config settings from conf file
             */

            getConfigFromFile(conffile);

            free(conffile);

            /* mysql connection */
            DBGL("Setting up mysql connect to %s",configValue("dbhost"));
            if((myconn = mysql_init(NULL)) == NULL ){
                CCAE(1,"cannot initialise connection to mysql libraries.");
            }
            DBGL("connecting to mysql: %s, db: %s, user: %s, pass: %s",configValue("dbhost"),configValue("dbdb"),configValue("dbuser"),configValue("dbpass"));
            if(mysql_real_connect(myconn,configValue("dbhost"),configValue("dbuser"),configValue("dbpass"),configValue("dbdb"),0,NULL,0)==NULL){
                CCAE(1,"cannot connect to mysql server.");
            }

            /* start dvbstreamer */
            startDvbStreamer(0);
            startDvbStreamer(1);

            /* enter the main loop */
            exitcode=mainLoop();

            DBG("loop completed.  exiting");

            /* stop dvbstreamer */
            stopDvbStreamer(0);
            stopDvbStreamer(1);

            /* stop mysql */
            DBG("freeing mysql library stuff");
            mysql_close(myconn);

            /* main loop completed, time to shut down */
            deleteConfig();

            DBG("Freeing signal handling struct.");
            free(siga);
        }else{
            DBG("Failed to setup default config");
        }
    }else{
        /* error allocating memory for argtable entries */
        CCAE(1,"Command line processing: Out of memory");
    }
    return exitcode;
}/*}}}*/
