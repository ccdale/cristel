/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbctrl.c
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Sunday 27 July 2014, 06:07:48
 * Last Modified: Monday  7 March 2016, 12:08:28
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

#include "dvbctrl.h"

static struct StreamerData SD;

void setupConnect(int adaptornum)/*{{{*/
{
    prepare_streamer_data();
    dvbc_connect(adaptornum);
}/*}}}*/
void closeConnect()/*{{{*/
{
    free_streamer_data();
}/*}}}*/
void prepare_streamer_data(void)/*{{{*/
{
    SD.socketfp=NULL;
    SD.connected=0;
    SD.authenticated=0;
    SD.errornumber=0;
    /*
    SD.ver=xmalloc(10);
    SD.errmsg=xmalloc(MAX_LINE_LENGTH);
    SD.errmsg[0]='\0';
    */
    SD.line=xmalloc(MAX_LINE_LENGTH);
    SD.line[0]='\0';
    SD.data=xmalloc(RCV_BUFFER_LENGTH);
    SD.data[0]='\0';
}/*}}}*/
void free_streamer_data(void)/*{{{*/
{
    if(SD.socketfp){
        DEBUG("Closing socket");
        fclose(SD.socketfp);
    }
    /*
    if(SD.ver){
        DEBUG("free SD.ver: %s",SD.ver);
        free(SD.ver);
    }
    if(SD.errmsg){
        DEBUG("free SD.errmsg: %s",SD.errmsg);
        free(SD.errmsg);
    }
    */
    if(SD.line){
        DEBUG("free SD.line: %s",SD.line);
        free(SD.line);
    }
    if(SD.data){
        DEBUG("free SD.data: %s",SD.data);
        free(SD.data);
    }
}/*}}}*/
int dvbc_connect(int adaptornum)/*{{{*/
{
    socklen_t address_len;
    struct sockaddr_storage address;
    struct addrinfo *addrinfo, hints;
    char portnumber[10];    
    int socketfd = -1;
    char *username;
    char *password;
    char *host;

    host=configValue("dvbhost");
    username=configValue("dvbuser");
    password=configValue("dvbpass");
    sprintf(portnumber, "%d", REMOTEINTERFACE_PORT + adaptornum);
    memset((void *)&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG;
    if ((getaddrinfo(host, portnumber, &hints, &addrinfo) != 0) || (addrinfo == NULL))
    {
        WARN("Failed to get address");
        return 1;
    }
    if (addrinfo->ai_addrlen > sizeof(struct sockaddr_storage))
    {
        WARN("Failed to parse address");
        freeaddrinfo(addrinfo);
        return 1;
    }
    address_len = addrinfo->ai_addrlen;
    memcpy(&address, addrinfo->ai_addr, addrinfo->ai_addrlen);
    freeaddrinfo(addrinfo);
    socketfd = socket(address.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (socketfd < 0)
    {
        WARN("Failed to create socket.");
        return 1;
    }
    if (connect(socketfd, (const struct sockaddr *) &address, address_len))
    {
        WARN("Failed to connect to host %s port %d", host, REMOTEINTERFACE_PORT + adaptornum);
        return 1;
    }
    INFO("Socket connected to host %s port %d", host, REMOTEINTERFACE_PORT + adaptornum);
    SD.socketfp = fdopen(socketfd, "r+");
    SD.connected=1;
    rcvData();
    if(SD.errornumber != 0){
        WARN("%s",SD.errmsg);
        return SD.errornumber;
    }
    SD.authenticated=Authenticate(username,password);
    return 0;
}/*}}}*/
int Authenticate(char *username, char *password)/*{{{*/
{
    sprintf(SD.line, "auth %s %s", username, password);
    request(SD.line);
    return (SD.errornumber==0);
}/*}}}*/
int sendData(char *data)/*{{{*/
{
    int len=strlen(data);

    if(SD.connected){
        if(len){
            DEBUG("Sending data '%s'",data);
            if(data[len-1]=='\n'){
                fprintf(SD.socketfp,"%s",data);
            }else{
                fprintf(SD.socketfp,"%s\n",data);
            }
        }
    }
    return len;
}/*}}}*/
int request(char *cmd)/*{{{*/
{
    int numlines=0;
    size_t len=0;

    DEBUG("in request: cmd %s",cmd)
    len=sendData(cmd);
    if(len==strlen(cmd)){
        numlines=rcvData();
    }
    return numlines;
}/*}}}*/
int rcvData(void)/*{{{*/
{
    char *seperator;
    char *start;
    int numlines=0;
    char *responselineStart;
    int rls;

    responselineStart=configValue("dvbbanner");
    rls=strlen(responselineStart);
    if(SD.connected){
        DEBUG("rcvData: socket is connected");
        while(!feof(SD.socketfp)){
            if(fgets(SD.line,MAX_LINE_LENGTH,SD.socketfp)){
                DEBUG("rcvd: %s",SD.line);
                if(strncmp(SD.line,responselineStart,rls-1)==0){
                    /* last line */
                    chomp(SD.line);
                    seperator=strchr(SD.line+rls,'/');
                    if(seperator){
                        *seperator=0;
                        start=SD.line+rls;
                        /* SD.ver=strdup(start); */
                        SD.ver=start;
                        start=seperator+1;
                        seperator=strchr(seperator+1,' ');
                        if(seperator)
                        {
                            *seperator=0;
                            /* SD.errmsg=strdup(seperator+1); */
                            SD.errmsg=seperator+1;
                        }
                        SD.errornumber=atoi(start);
                        numlines++;
                    }
                    break;
                }else{
                    /* data lines */
                    addLineToBuffer();
                    numlines++;
                }
            }
        }
        if(feof(SD.socketfp)){
            DEBUG("rcvData: socket feof");
        }
    }else{
        WARN("rcvData: SD is not connected");
    }
    DEBUG("received %d lines",numlines);
    return numlines;
}/*}}}*/
void addLineToBuffer()/*{{{*/
{
    int cn;
    int bufflen;

    bufflen=strlen(SD.data);
    cn=bufflen+strlen(SD.line);
    if(cn<RCV_BUFFER_LENGTH){
        sprintf(SD.data+bufflen,"%s",SD.line);
    }else{
        WARN("buffer is undersized");
        DEBUG("%s",SD.line);
    }
}/*}}}*/
struct ServiceInfo *newServiceInfo(void)/*{{{*/
{
    struct ServiceInfo *SI;

    SI=xmalloc(sizeof(struct ServiceInfo));
    SI->type=0;
    SI->mux=0;
    SI->pmtpid=0;
    SI->name=NULL;
    SI->source=NULL;
    SI->ID=NULL;
    return SI;
}/*}}}*/
void freeServiceInfo(struct ServiceInfo *SI)/*{{{*/
{
    if(SI){
        if(SI->name){
            free(SI->name);
        }
        if(SI->source){
            free(SI->source);
        }
        if(SI->ID){
            free(SI->ID);
        }
        free(SI);
    }
}/*}}}*/
struct ServiceInfo *serviceInfoParse(char *si)/*{{{*/
{
    /*
     * Name                : "BBC TWO"
     * Provider            : "(null)"
     * Type                : Digital TV
     * Conditional Access? : Free to Air
     * ID                  : 233a.104d.10bf
     * Multiplex UID       : 1454726187
     * Source              : 0x10bf
     * Default Authority   : "fp.bbc.co.uk"
     * PMT PID             : 0x00c8
     */
    struct ServiceInfo *SI;
    struct ColonParse *CP;
    char *line;
    char *linetok="\n";
    char *linesave;

    SI=newServiceInfo();

    /* split it up into lines */
    line=strtok_r(si,linetok,&linesave);
    while(line){
        /* split each line at the colon */
        CP=parseColon(line);
        updateServiceInfo(SI,CP->key,CP->val);
        if(CP->tmp){
            free(CP->tmp);
        }
        free(CP);
        /* next line */
        line=strtok_r(NULL,linetok,&linesave);
    }
    return SI;
}/*}}}*/
struct ColonParse *parseColon(char *line)/*{{{*/
{
    char *word;
    char *wordtok=":";
    char *wordsave;

    struct ColonParse *CP;

    CP=xmalloc(sizeof(struct ColonParse));

    word=strtok_r(line,wordtok,&wordsave);
    CP->tmp=strdup(word);
    CP->key=trim(CP->tmp);
    word=strtok_r(NULL,wordtok,&wordsave);
    CP->val=lefttrim(righttrim(trim(word),'"'),'"');
    return CP;
}/*}}}*/
void updateServiceInfo(struct ServiceInfo *SI,char *key,char *val)/*{{{*/
{
    DEBUG("Parse: key: %s, val: %s",key,val);
    if(strcmp(key,"Name")==0){
        SI->name=strdup(val);
        DEBUG("Service Name: %s",SI->name);
    }else if(strcmp(key,"Type")==0){
        if(strcmp(val,"Digital TV")==0){
            SI->type=1;
            DEBUG("Type: Digital TV");
        }else if(strcmp(val,"Digital Radio")==0){
            SI->type=2;
            DEBUG("Type: Digital Radio");
        }else if(strcmp(val,"Data")==0){
            SI->type=3;
            DEBUG("Type: Data");
        }else{
            SI->type=255;
            WARN("Type: Unknown type: key: %s, val: %s",key,val);
        }
    }else if(strcmp(key,"Conditional Access?")==0){
        if(strcmp(val,"Free to Air")==0){
            SI->ca=1;
            DEBUG("CA: Free to Air");
        }else{
            SI->ca=255;
            DEBUG("CA: Encrypted");
        }
    }else if(strcmp(key,"ID")==0){
        SI->ID=strdup(val);
        DEBUG("ID: %s",SI->ID);
    }else if(strcmp(key,"Multiplex UID")==0){
        SI->mux=atoi(val);
        DEBUG("Mux: %d",SI->mux);
    }else if(strcmp(key,"Source")==0){
        SI->source=strdup(val);
        DEBUG("Source: %s",SI->source);
    }else if(strcmp(key,"PMT PID")==0){
        SI->pmtpid=atoi(val);
        DEBUG("PMT PID %d",SI->pmtpid);
    }
}/*}}}*/
struct ServiceInfo *getServiceInfo(char *service)/*{{{*/
{
    struct ServiceInfo *SI;
    char *si;
    char *cmd;

    cmd=xmalloc(MAX_LINE_LENGTH);
    sprintf(cmd,"serviceinfo '%s'",service);
    si=dvbcommand(cmd,0);
    SI=serviceInfoParse(si);
    free(si);
    free(cmd);
    return SI;
}/*}}}*/
char * dvbcommand(char *cmd,int adaptornum)/*{{{*/
{
    char *output=NULL;
    int nl;

    setupConnect(adaptornum);
    if(SD.authenticated){
        sprintf(SD.line,"%s",cmd);
        nl=request(SD.line);
        if(nl>0){
            output=strdup(SD.data);
        }
    }
    closeConnect();
    return output;
}/*}}}*/
