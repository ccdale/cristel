/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * tools.c
 *
 * Started: Wednesday 21 November 2012, 10:46:01
 * Last Modified: Friday 25 July 2014, 09:31:47
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
#include "tools.h"

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
    if((top=malloc(length))){
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
        if((op=malloc(++current))){
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

    tim=(struct tm *) malloc(sizeof(struct tm));
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
    if((tbname=malloc(len)) == NULL){
        CCAE(1,"out of memory");
    }
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

    statbuf=malloc(sizeof(struct stat));
    if(statbuf){
        /* syslog(LOG_DEBUG,"in filesize, malloced statbuf, statting"); */
        ret=stat(filename,statbuf);
        if(ret==0){
            /* syslog(LOG_DEBUG,"stat ok. reading file size"); */
            fsize=statbuf->st_size;
            /* syslog(LOG_DEBUG,"file size: %ld",fsize); */
        }else{
            syslog(LOG_ERR,"Cannot read file %s",filename);
        }
        /* syslog(LOG_DEBUG,"freeing stat buffer"); */
        free(statbuf);
    }else{
        syslog(LOG_ERR,"Cannot allocate memory for stat buffer");
    }
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
    if((nsp=malloc(slen+1))){
        junk=strcpy(nsp,str);
        if(junk!=nsp){
            syslog(LOG_ERR,"failure in copying string.");
            free(nsp);
            nsp=NULL;
        }
    }else{
        syslog(LOG_ERR,"cannot allocate memory for new string pointer.");
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
    buffer=malloc(length);
    if(buffer){
        length=snprintf(buffer,length,"%lld",num);
        return buffer;
    }
    return NULL;
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
