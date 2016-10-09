/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * tools.c
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Wednesday 21 November 2012, 10:46:01
 * Version: 0.00
 * Revision: $Id: tools.c 55 2013-03-24 21:48:39Z chris.charles.allison@gmail.com $
 * Last Modified: Sunday  9 October 2016, 09:30:16
 */

#include "tools.h"

void *xmalloc(size_t size)/*{{{*/
{
    void *xmem;
    xmem=malloc(size);
    if(xmem!=NULL){
        return xmem;
    }
    /* exit due to allocation error */
    CCAE(1,"Failed to allocate %d bytes of memory",(int)size);
}/*}}}*/
void *xcalloc(size_t nmemb, size_t size)/*{{{*/
{
    void *xmem;

    xmem=xmalloc(nmemb*size);
    xmem=memset(xmem,0,nmemb*size);
    return xmem;
}/*}}}*/
char *hms(int seconds)/* {{{1 */
{
    int days=0, hours=0, mins=0, secs=0;
    char *output;

    if(seconds>86400){
        days=seconds / 86400;
        secs=seconds % 86400;
    }else{
        secs=seconds;
    }
    if(secs>3600){
        hours=secs / 3600;
        secs=secs % 3600;
    }
    if(secs>60){
        mins=secs / 60;
        secs=secs % 60;
    }
    if(days>1){
        output=fitstring("%d days, %.2d:%.2d:%.2d",days,hours,mins,secs);
    }else if(days==1){
        output=fitstring("%d day, %.2d:%.2d:%.2d",days,hours,mins,secs);
    }else{
        output=fitstring("%.2d:%.2d:%.2d",hours,mins,secs);
    }
    return output;
}/* }}} */
char *fitstring(char *str, ...)/*{{{*/
{
    char *xstr;
    int slen;
    va_list args;

    va_start(args,str);
    slen=vsnprintf(0,0,str,args);
    va_end(args);
    if(slen<0){
        WARN("fitstring: failed! string: '%s', returned %d",str,slen);
        return NULL;
    }
    /* DEBUG("fitstring: length: %d",slen);*/
    if(slen>0){
        xstr=xmalloc(++slen);
    }else{
        WARN("fitstring: failed! string: '%s', returned %d",str,slen);
        return NULL;
    }
    va_start(args,str);
    slen=vsnprintf(xstr,slen,str,args);
    va_end(args);
    return xstr;
}/*}}}*/
char *concatFileParts(int numparts, ...)/* {{{1 */
{
    va_list valist;
    int x;
    char *tmp;
    char *buffer;
    int fnlen=0;

    buffer=xmalloc(PATH_MAX);
    *buffer='\0';
    va_start(valist, numparts);
    for(x=0;x<numparts;x++){
        tmp=va_arg(valist, char*);
        /* DEBUG("variable arg %d: %s",x,tmp);*/
        fnlen+=strlen(tmp);
        if(fnlen<PATH_MAX){
            strcat(buffer,tmp);
        }else{
            CCAE(1,"Filename length exceeded: %s + %s",buffer,tmp);
        }
    }
    va_end(valist);
    return buffer;
}/* }}} */
int filenumberFromFilename(char *filename)/* {{{1 */
{
    /*
     * attempts to retrieve the file number from the filename
     * of the form filename.<number>.ext
     * returns the number found.
     * there is no error checking, if the second token is not 
     * a number strange things will happen.
     */
    const char dot[]=".";
    char *str;
    char *token;
    int ret=-1;

    if((str=strdup(filename)) == NULL){
        CCAE(1,"out of memory.");
    }
    /* find the first part of the filename */
    token=strtok(str,dot);
    /* DBGL("first token: %s",token); */
    /* now the original ext */
    token=strtok(NULL,dot);
    /* DBGL("second token: %s",token); */
    /* now the numeric part */
    token=strtok(NULL,dot);
    /* DBGL("third token: %s",token); */
    ret=atoi(token);
    free(str);
    return ret;
}/* }}} */
char *sensibleFilename(char *str)/* {{{1 */
{
    char *fnstr,*tmpstr;
    tmpstr=replaceSpaces(str);
    if(tmpstr){
        /* DEBUG("replaceSpaces has returned: '%s'",tmpstr);*/
        fnstr=sensibleChars(tmpstr);
        /* DEBUG("sensibleChars has returned: '%s'",fnstr);*/
        free(tmpstr);
    }
    return fnstr;
}/* }}} */
char *replaceSpaces(char *str)/*{{{*/
{
    int outpipe[2];
    int inpipe[2];
    char *fnstr=NULL;
    char *tmp;
    int slen,rlen;
    int cpid;
    int status;

    if((slen=strlen(str))>0){
        fnstr=xcalloc(sizeof(char),++slen);
        if(pipe(outpipe)<0){
            WARN("sensible filename failed for '%s' at create out pipe",str);
        }
        if(pipe(inpipe)<0){
            WARN("sensible filename failed for '%s' at create in pipe",str);
        }
        if((cpid=fork())<0){
            WARN("sensible filename failed to fork for '%s'",str);
            close(outpipe[0]);
            close(outpipe[1]);
            close(inpipe[0]);
            close(inpipe[1]);
        }else if(cpid==0){
            /* child process */
            close(inpipe[1]); /* don't need in pipe write end */
            close(outpipe[0]); /* nor outpipe's read end */
            if(dup2(inpipe[0],STDIN_FILENO) < 0){
                CCAE(1,"sensible filename failed to duplicate stdin for forked process");
            }
            if(dup2(outpipe[1],STDOUT_FILENO) < 0){
                CCAE(1,"sensible filename failed to duplicate stdout for forked process");
            }
            /* convert spaces to underscores */
            execlp("/usr/bin/tr","/usr/bin/tr","[ ]","_",NULL);
            close(inpipe[0]);
            close(outpipe[1]);
            CCAE(1,"error executing /usr/bin/tr");
        }else{
            /* parent process continues */
            close(inpipe[0]);
            close(outpipe[1]);
            /* DEBUG("writing str to tr: '%s'",str);*/
            write(inpipe[1],str,slen);
            close(inpipe[1]);
            /* DEBUG("reading from tr into fnstr");*/
            rlen=read(outpipe[0],fnstr,slen);
            close(outpipe[0]);
            /* DEBUG("read %d bytes from tr",rlen);*/
            /* DEBUG("adding terminating zero");*/
            tmp=fnstr+rlen;
            *tmp='\0';
            /* DEBUG("read: '%s'",fnstr);*/
            /* DEBUG("Parent will now wait for tr process to finish");*/
            wait(&status);
        }
    }
    return fnstr;
}/*}}}*/
char *sensibleChars(char *str)/*{{{*/
{
    int outpipe[2];
    int inpipe[2];
    char *fnstr=NULL;
    char *tmp;
    int slen,rlen;
    int cpid;
    int status;

    if((slen=strlen(str))>0){
        fnstr=xcalloc(sizeof(char),++slen);
        if(pipe(outpipe)<0){
            WARN("sensible chars failed for '%s' at create out pipe",str);
        }
        if(pipe(inpipe)<0){
            WARN("sensible chars failed for '%s' at create in pipe",str);
        }
        if((cpid=fork())<0){
            WARN("sensible chars failed to fork for '%s'",str);
            close(outpipe[0]);
            close(outpipe[1]);
            close(inpipe[0]);
            close(inpipe[1]);
        }else if(cpid==0){
            /* child process */
            close(inpipe[1]); /* don't need in pipe write end */
            close(outpipe[0]); /* nor outpipe's read end */
            if(dup2(inpipe[0],STDIN_FILENO) < 0){
                CCAE(1,"sensible chars failed to duplicate stdin for forked process");
            }
            if(dup2(outpipe[1],STDOUT_FILENO) < 0){
                CCAE(1,"sensible chars failed to duplicate stdout for forked process");
            }
            /* convert spaces to underscores */
            /* DEBUG("running: tr -cd '[a-zA-Z0-9_-]'");*/
            execlp("/usr/bin/tr","/usr/bin/tr","--complement","--delete","[a-zA-Z0-9]_-",NULL);
            close(inpipe[0]);
            close(outpipe[1]);
            CCAE(1,"error executing /usr/bin/tr");
        }else{
            /* parent process continues */
            close(inpipe[0]);
            close(outpipe[1]);
            /* DEBUG("writing str to tr: '%s'",str);*/
            write(inpipe[1],str,slen);
            close(inpipe[1]);
            /* DEBUG("reading from tr into fnstr");*/
            rlen=read(outpipe[0],fnstr,slen);
            close(outpipe[0]);
            /* DEBUG("read %d bytes from tr",rlen);*/
            /* DEBUG("adding terminating zero");*/
            tmp=fnstr+rlen;
            *tmp='\0';
            /* DEBUG("read: '%s'",fnstr);*/
            wait(&status);
        }
    }
    return fnstr;
}/*}}}*/
char *escapestr(char *str)/* {{{1 */
{
    char *op=NULL;
    char *top;
    char *iter;
    char *titer;
    long length;
    long current=0;

    length=strlen(str);
    length=length*2; /* double the amount of space should be sufficient */
    if((top=xmalloc(length))){
        titer=top;
        iter=str;
        while(iter != '\0' && current < length){
            switch(*iter){
                case '\"': *titer='\\'; titer++; *titer='\"';titer++;current+=2; break;
                case '\'': *titer='\\'; titer++; *titer='\'';titer++;current+=2; break;
                default: *titer=*iter; titer++; current++; break;
            }
            iter++;
        }
        titer='\0';
        if((op=xmalloc(++current))){
            strcpy(op,top);
            free(top);
        }else{
            CCAE(1,"cannot allocate memory for output escaped string: %s",top);
        }
    }else{
        CCAE(1,"Cannot allocate memory for escaping string: %s",str);
    }
    return op;
}
struct tm *initTm(void)/* {{{1 */
{
    struct tm *tim;

    tim=(struct tm *) xmalloc(sizeof(struct tm));
    if(tim){
        tim->tm_sec=0;
        tim->tm_min=0;
        tim->tm_hour=0;
        tim->tm_mday=0;
        tim->tm_mon=0;
        tim->tm_year=0;
        tim->tm_wday=0;
        tim->tm_yday=0;
        tim->tm_isdst=0;
    }
    return tim;
}/* }}} */
char *bname(char *fqfilename)/* {{{1 */
{
    /*
     * returns a pointer to a new string
     * which is the basename of the 
     * fully qualified filename supplied
     */
    char *tmp;
    char *tbname=NULL;
    int len;

    DBGL("Finding basename of %s",fqfilename);
    tmp=basename(fqfilename);
    DBGL("Found %s",tmp);
    /* find out how big a string we need */
    len=snprintf(tbname,0,"%s",tmp);
    /* + terminating null */
    len++;
    tbname=xmalloc(len);
    len=snprintf(tbname,len,"%s",tmp);
    return tbname;
}/* }}} */
long filesize(char *filename)/* {{{1 */
{
    /*
     * returns the filesize of filename if it can be read
     * returns -1 if an error occured.
     */
    int ret;
    long fsize=-1;
    struct stat *statbuf;

    statbuf=xmalloc(sizeof(struct stat));
    /* syslog(LOG_DEBUG,"in filesize, malloced statbuf, statting"); */
    ret=stat(filename,statbuf);
    if(ret==0){
        /* syslog(LOG_DEBUG,"stat ok. reading file size"); */
        fsize=statbuf->st_size;
        /* syslog(LOG_DEBUG,"file size: %ld",fsize); */
    }else{
        /* it is not an error if the file cannot be
         * read, if this function is used as a 
         * file_exists function
         */
        DEBUG("file does not exist: %s",filename);
    }
    /* syslog(LOG_DEBUG,"freeing stat buffer"); */
    free(statbuf);
    /* syslog(LOG_DEBUG,"returning from filesize: %ld",fsize); */
    return fsize;
}/* }}} */
char *newstringpointer(char *str)/* {{{1 */
{
    /* don't use this, use strdup(3) instead */
    char *nsp;
    char *junk;
    int slen=0;
    slen=strlen(str);
    nsp=xmalloc(slen+1);
    junk=strcpy(nsp,str);
    if(junk!=nsp){
        syslog(LOG_ERR,"failure in copying string.");
        free(nsp);
        nsp=NULL;
    }
    return nsp;
}/* }}} */
long strtolong(char *str)/* {{{1 */
{
    long val;
    char *endptr;

    errno=0;
    val=strtol(str,&endptr,10); /* base 10 always */
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) {
        CCAE(1,"strtol conversion of %s",str);
    }
    if(endptr == str){
        CCAE(1,"strtol no digits found in %s",str);
    }
    return val;
}/* }}} */
char *numtostr(long long num)/* {{{1 */
{
    size_t length;
    char *buffer=NULL;
    length=snprintf(buffer,0,"%lld",num);
    length++;
    buffer=xmalloc(length);
    length=snprintf(buffer,length,"%lld",num);
    return buffer;
}/* }}} */
char *trim(char *str)/*{{{1*/
{
    char *string;
    string=chomp(str);
    string=ltrim(string);
    return rtrim(string);
}/*}}}*/
char *chomp(char *str) /* {{{1 */
{
    int len;
    int pos;

    len=strlen(str);
    pos=strcspn(str,"\n");
    if(pos<len){
        str[pos]='\0';
    }
    return str;
} /* }}} */
char *ltrim(char *str)/* {{{1 */
{
    char *string;
    string=ltrimsp(str);
    return ltrimt(string);
}/* }}} */
char *rtrim(char *str)/* {{{1 */
{
    char *string;
    string=rtrimsp(str);
    return rtrimt(string);
}/* }}} */
char *ltrimsp(char *str)/* {{{1 */
{
    return lefttrim(str,' ');
}/* }}} */
char *rtrimsp(char *str)/* {{{1 */
{
    return righttrim(str,' ');
}/*}}}*/
char *ltrimt(char *str)/*{{{1*/
{
    return lefttrim(str,'\t');
}/*}}}*/
char *rtrimt(char *str)/*{{{1*/
{
    return righttrim(str,'\t');
}/*}}}*/
char* lefttrim(char *string, char junk)/* {{{1 */
{
    char* original = string;
    char *p = original;
    int trimmed = 0;
    do
    {
        if (*original != junk || trimmed)
        {
            trimmed = 1;
            *p++ = *original;
        }
    }
    while (*original++ != '\0');
    return string;
}/* }}} */
char *righttrim(char *string, char junk)/* {{{1 */
{
    char *original = string + strlen(string);
    while (original != string && *--original == junk)
        ;

    if (*original != '\0')
        original[*original == junk ? 0 : 1] = '\0';
    return string;
}/* }}} */
int readPidFile(char *fn)/*{{{*/
{
    int fsz;
    FILE *fp;
    char *buf;
    char *test;
    int pid=-1;

    buf=xmalloc(MAX_LINE_LENGTH);
    if((fsz=filesize(fn))>0){
        if((fp=fopen(fn,"r"))){
            test=fgets(buf,MAX_LINE_LENGTH,fp);
            if(test==buf){
                pid=atoi(buf);
            }
            fclose(fp);
        }
    }
    free(buf);
    return pid;
}/*}}}*/
int numLines(char *lines)/*{{{*/
{
    char *tok="\n";
    char *tlines;
    char *hlines;
    char *token;
    int nlines=0;

    tlines=strdup(lines);
    hlines=tlines;
    token=strchr(tlines,'\n');
    if(token){
        token=strtok(tlines,tok);
        while(token){
            nlines++;
            token=strtok(NULL,tok);
        }
    }
    free(hlines);
    return nlines;
}/*}}}*/
