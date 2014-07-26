/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * configfile.c
 *
 * Started: Sunday 23 December 2012, 09:47:48
 * Last Modified: Saturday 26 July 2014, 04:42:06
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

#include "configfile.h"

int initConfig( void )/* {{{1 */
{
    if((configuration=malloc(sizeof(struct ConfigItem)))==NULL){
        CCA_ERR_EXIT(1,"Unable to allocate memory for the configuration.");
    }
    configuration->key=NULL;
    configuration->value=NULL;
    configuration->next=NULL;
    DBG("Configuration initialised");
    return 0;
}/* }}} */
void getConfigFromFile(char *filename)/* {{{1 */
{
    /*
     * this assumes that the filename is a valid string
     * if it is also a valid path to a file and that file is readable
     * then it will read through it. Any lines that are not blank nor start with a '#'
     * after trimming, are parsed into key=value pairs and added to the config list.
     * if there is no '=' then the key becomes the whole line and the value is NULL
     * each sub string is also passed through the trim function to chop of leading and 
     * trailing spaces and any trailing new line.
     */
    long fsize;
    long slen;
    FILE *cfp;
    int bufsize=1024;
    char *buffer;
    char *str;
    char *key;
    char *tkey;
    char *value;
    char *tv;
    /* struct ConfigItem *tc=NULL; */
    char equals[]="=";
    char hash[]="#";

    if((buffer=malloc(bufsize))==NULL){
        CCA_ERR_EXIT(5,"Unable to allocate memory for config file buffer.");
    }
    if((fsize=filesize(filename))==-1){
        CCA_ERR_EXIT(3,"Cannot read config filename provided.");
    }
    cfp=fopen(filename,"r");
    if(cfp){
        while((str=fgets(buffer,bufsize,cfp))!=NULL){
            /* trim off any leading and trailing white space and the trailing new line */
            str=trim(buffer);
            /* check for the empty string */
            if((slen=strlen(str))>0){
                /* check for commented lines */
                if(str[0]!='#'){
                    /*
                     * if strtok returns NULL then there is no = sign on the line
                     * so the key becomes the whole line and
                     * the value is set to NULL
                     */
                    if((tkey=strtok(str,equals))==NULL){
                        if(( key=strdup(str)) == NULL){
                            CCAE(1,"out of memory attempting to duplicate %s.",str);
                        }else{
                            value=NULL;
                        }
                    }else{
                        /* trim each part as well */
                        tkey=rtrim(tkey);
                        if(( key=strdup(tkey)) == NULL){
                            CCAE(1,"out of memory attempting to duplicate %s.",str);
                        }else{
                            if((value=strtok(NULL,equals))!=NULL){
                                /* check for end of line comments */
                                if((tv=strtok(value,hash))!=NULL){
                                    if(( value=strdup(trim(tv))) == NULL){
                                        CCAE(1,"out of memory attempting to duplicate %s.",str);
                                    }
                                }else{
                                    if(( value=strdup(trim(value))) == NULL){
                                        CCAE(1,"out of memory attempting to duplicate %s.",str);
                                    }
                                }
                            }
                        }
                    }
                    updateConfig(key,value);
                }
            }
        }
        fclose(cfp);
    }else{
        CCA_ERR_EXIT(4,"Cannot open config file for reading.");
    }
    free(buffer);
}/* }}} */
struct ConfigItem *addConfig(char *key, char *value)/* {{{1 */
{
    struct ConfigItem *tc;

    printf("adding config item %s = %s\n",key,value);
    if(configuration->key==NULL){
        /* configuration has been initialised but is empty */
        configuration->key=key;
        configuration->value=value;
    }else{
        if(( tc = (struct ConfigItem*) malloc( sizeof(struct ConfigItem))) == NULL){
            CCA_ERR_EXIT(2,"Unable to allocate memory for a new config item.");
        }
        tc->key=key;
        tc->value=value;
        /* this will now become the front of the list */
        tc->next=configuration;
        configuration=tc;
    }
    return configuration;
}/* }}} */
struct ConfigItem *updateConfig(char *key, char *value)/* {{{1 */
{
    struct ConfigItem *tc;

    if(( tc = searchConfigP( key )) == NULL){
        tc=addConfig( key, value);
    }else{
        free(tc->key);
        tc->key=key;
        free(tc->value);
        tc->value=value;
    }
    return tc;
}/* }}} */
int deleteConfigItem(char *key)/* {{{1 */
{
    struct ConfigItem *tc;
    struct ConfigItem *prev;
    int junk;
    int foundit=0;

    prev=NULL;
    tc=configuration;
    /* walk the configuration tree */
    while(tc->next && (foundit == 0)){
        if(( junk = strcmp( tc->key, key )) == 0){
            /* found the one to delete
             * tc is the one to delete
             * prev is either NULL if this is the first one in the list
             * or the address of the previous one
             */
            foundit=1;
        }else{
            /* this is not the one we want
             * prev becomes this one
             * tc becomes the next one
             */
            prev=tc;
            tc=tc->next;
        }
    }
    /*
     * did we find it yet?
     */
    if(foundit==0){
        /* test the last one then */
        if(( junk = strcmp( tc->key, key )) == 0){
            foundit=1;
        }
    }
    /*
     * have we found it now?
     */
    if(foundit==1){
        /*
         * yes we have the one we wish to delete
         * so, delete it.
         */
        unlinkConfig(prev,tc);
    }
    return foundit;
}/* }}} */
void unlinkConfig(struct ConfigItem *prev, struct ConfigItem *dc)/* {{{1 */
{
    /*
     * this deletes the config item dc by freeing up the memory it uses.
     * it closes the gap in the configuration tree
     * or chops of the head item or the tail item
     */
    if(dc->next){
        if(prev==NULL){
            configuration=dc->next;
        }else{
            prev->next=dc->next;
        }
        free(dc->key);
        free(dc->value);
        free(dc);
    }
}/* }}} */
void deleteConfig( void )/* {{{1 */
{
    struct ConfigItem *tc;
    tc=configuration;
    while(tc->next){
        printf("freeing config item %s\n",tc->key);
        configuration=tc->next;
        if(tc->key){
            free(tc->key);
        }
        if(tc->value){
            free(tc->value);
        }
        if(tc){
            free(tc);
        }
        tc=configuration;
    }
    /* ok, at the last one */
    printf("freeing config item %s\n",configuration->key);
    if(configuration->key){
        free(configuration->key);
    }
    if(configuration->value){
        free(configuration->value);
    }
    free(configuration);
}/* }}} */
struct ConfigItem *searchConfigP(char *key)/* {{{1 */
{
    struct ConfigItem *tc=configuration;
    int junk;

    /* tc=configuration; */
    /* walk the configuration tree */
    while(tc->next){
        if(tc->key){
            if((junk=strcmp(tc->key,key))==0){
                return tc;
            }
        }
        tc=tc->next;
    }
    /* last one */
    if(tc->key){
        if((junk=strcmp(tc->key,key))==0){
            return tc;
        }
    }
    return NULL;
}/* }}} */
char *configValueDup(char *key)/* {{{1 */
{
    char *tstr;
    char *ret;

    tstr=configValue(key);
    if(tstr){
        if((ret=strdup(tstr)) == NULL ){
            CCAE(1,"Out of memory duplicating %s (key:%s)",tstr,key);
        }
    }else{
        ret=NULL;
    }
    return ret;
}/* }}} */
char *configValue(char *key)/* {{{1 */
{
    struct ConfigItem *tc;

    tc=searchConfigP(key);
    if(tc!=NULL){
        return tc->value;
    }
    return NULL;
}/* }}} */
void printConfig( void )/* {{{1 */
{
    struct ConfigItem *tc;
    tc=configuration;
    while(tc->next){
        printf("key: /%s/ = value: /%s/\n",tc->key,tc->value);
        tc=tc->next;
    }
    printf("key: /%s/ = value: /%s/\n",tc->key,tc->value);
}/* }}} */
