/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbctrl.c
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Sunday 27 July 2014, 06:07:48
 * Last Modified: Friday  8 January 2016, 04:37:01
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

static struct StreamerData SD[2];

static void setupConnect(int adaptornum)/*{{{*/
{
    prepare_streamer_data(adaptornum);
    dvbc_connect(adaptornum);
}/*}}}*/
static void closeConnect(int adaptornum)/*{{{*/
{
    free_streamer_data(adaptornum);
    SD[adaptornum].connected=0;
    SD[adaptornum].authenticated=0;
}/*}}}*/
static void prepare_streamer_data(int adaptornum)/*{{{*/
{
    SD[adaptornum].socketfp=NULL;
    SD[adaptornum].connected=0;
    SD[adaptornum].authenticated=0;
    SD[adaptornum].errornumber=0;
    SD[adaptornum].ver=xmalloc(10);
    SD[adaptornum].errmsg=xmalloc(MAX_LINE_LENGTH);
    SD[adaptornum].line=xmalloc(MAX_LINE_LENGTH);
    SD[adaptornum].data=xmalloc(RCV_BUFFER_LENGTH);
}/*}}}*/
static void free_streamer_data(int adaptornum)/*{{{*/
{
    if(SD[adaptornum].socketfp){
        DBG("Closing socket");
        fclose(SD[adaptornum].socketfp);
    }
    if(SD[adaptornum].ver){
        DBGL("free SD[adaptornum].ver: %s",SD[adaptornum].ver);
        free(SD[adaptornum].ver);
    }
    if(SD[adaptornum].errmsg){
        DBGL("free SD[adaptornum].errmsg: %s",SD[adaptornum].errmsg);
        free(SD[adaptornum].errmsg);
    }
    if(SD[adaptornum].line){
        DBGL("free SD[adaptornum].line: %s",SD[adaptornum].line);
        free(SD[adaptornum].line);
    }
    if(SD[adaptornum].data){
        DBGL("free SD[adaptornum].data: %s",SD[adaptornum].data);
        free(SD[adaptornum].data);
    }
}/*}}}*/
static int dvbc_connect(int adaptornum)/*{{{*/
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
    SD[adaptornum].socketfp = fdopen(socketfd, "r+");
    rcvData(adaptornum);
    if(SD[adaptornum].errornumber != 0){
        WARN("%s",SD[adaptornum].errmsg);
        return SD[adaptornum].errornumber;
    }
    SD[adaptornum].connected=1;
    SD[adaptornum].authenticated=Authenticate(adaptornum,username,password);
    return 0;
}/*}}}*/
static int Authenticate(int adaptornum, char *username, char *password)/*{{{*/
{
    sprintf(SD[adaptornum].line, "auth %s %s", username, password);
    request(adaptornum,SD[adaptornum].line);
    return (SD[adaptornum].errornumber==0);
}/*}}}*/
static int sendData(int adaptornum,char *data)/*{{{*/
{
    int len=strlen(data);

    if(SD[adaptornum].connected){
        if(len){
            DBGL("Sending data '%s'",data);
            if(data[len-1]=='\n'){
                fprintf(SD[adaptornum].socketfp,"%s",data);
            }else{
                fprintf(SD[adaptornum].socketfp,"%s\n",data);
            }
        }
    }
    return len;
}/*}}}*/
static int request(int adaptornum,char *cmd)/*{{{*/
{
    int numlines=0;
    size_t len=0;

    len=sendData(adaptornum,cmd);
    if(len==strlen(cmd)){
        numlines=rcvData(adaptornum);
    }
    return numlines;
}/*}}}*/
static int rcvData(int adaptornum)/*{{{*/
{
    char *seperator;
    char *start;
    int numlines=0;
    char *responselineStart;
    int rls;

    responselineStart=configValue("dvbbanner");
    rls=strlen(responselineStart);
    if(SD[adaptornum].connected){
        while(!feof(SD[adaptornum].socketfp)){
            if(fgets(SD[adaptornum].line,MAX_LINE_LENGTH,SD[adaptornum].socketfp)){
                if(strncmp(SD[adaptornum].line,responselineStart,rls-1)==0){
                    /* last line */
                    chomp(SD[adaptornum].line);
                    seperator=strchr(SD[adaptornum].line+rls,'/');
                    if(seperator){
                        *seperator=0;
                        start=SD[adaptornum].line+rls;
                        SD[adaptornum].ver=strdup(start);
                        start=seperator+1;
                        seperator=strchr(seperator+1,' ');
                        if(seperator)
                        {
                            *seperator=0;
                            SD[adaptornum].errmsg=strdup(seperator+1);
                        }
                        SD[adaptornum].errornumber=atoi(start);
                        numlines++;
                    }
                    break;
                }else{
                    /* data lines */
                    addLineToBuffer(adaptornum);
                    numlines++;
                }
            }
        }
    }
    return numlines;
}/*}}}*/
static void addLineToBuffer(int adaptornum)/*{{{*/
{
    int cn;
    int bufflen;

    bufflen=strlen(SD[adaptornum].data);
    cn=bufflen+strlen(SD[adaptornum].line);
    if(cn<RCV_BUFFER_LENGTH){
        sprintf(SD[adaptornum].data+bufflen,"%s\n",SD[adaptornum].line);
    }else{
        WARN("buffer is undersized");
        DBG(SD[adaptornum].line);
    }
}/*}}}*/
char * dvbcmd(int adaptornum, char *cmd)/*{{{*/
{
    char *response=NULL;
    int nl;

    setupConnect(adaptornum);
    if(SD[adaptornum].authenticated){
        sprintf(SD[adaptornum].line,"%s",cmd);
        nl=request(adaptornum,SD[adaptornum].line);
        if(nl>0){
            response=strdup(SD[adaptornum].data);
        }
    }
    closeConnect(adaptornum);
    return response;
}/*}}}*/
char * lsservices(int adaptornum)/*{{{*/
{
    char* cmd="lsservices";
    return dvbcmd(adaptornum,cmd);
}/*}}}*/
