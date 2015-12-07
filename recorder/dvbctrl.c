/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbctrl.c
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Sunday 27 July 2014, 06:07:48
 * Last Modified: Sunday  6 September 2015, 09:29:02
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

static void setupConnect(int adaptornum)/*{{{*/
{
    prepare_streamer_data();
    dvbc_connect(adaptornum);
}/*}}}*/
static void closeConnect()/*{{{*/
{
    free_streamer_data();
}/*}}}*/
static void prepare_streamer_data(void)/*{{{*/
{
    SD.socketfp=NULL;
    SD.connected=0;
    SD.authenticated=0;
    SD.errornumber=0;
    SD.ver=xmalloc(10);
    SD.errmsg=xmalloc(MAX_LINE_LENGTH);
    SD.line=xmalloc(MAX_LINE_LENGTH);
    SD.data=xmalloc(RCV_BUFFER_LENGTH);
}/*}}}*/
static void free_streamer_data(void)/*{{{*/
{
    if(SD.socketfp){
        DBG("Closing socket");
        fclose(SD.socketfp);
    }
    if(SD.ver){
        DBGL("free SD.ver: %s",SD.ver);
        free(SD.ver);
    }
    if(SD.errmsg){
        DBGL("free SD.errmsg: %s",SD.errmsg);
        free(SD.errmsg);
    }
    if(SD.line){
        DBGL("free SD.line: %s",SD.line);
        free(SD.line);
    }
    if(SD.data){
        DBGL("free SD.data: %s",SD.data);
        free(SD.data);
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
    SD.socketfp = fdopen(socketfd, "r+");
    rcvData();
    if(SD.errornumber != 0){
        WARN("%s",SD.errmsg);
        return SD.errornumber;
    }
    SD.connected=1;
    SD.authenticated=Authenticate(username,password);
    return 0;
}/*}}}*/
static int Authenticate(char *username, char *password)/*{{{*/
{
    sprintf(SD.line, "auth %s %s", username, password);
    request(SD.line);
    return (SD.errornumber==0);
}/*}}}*/
static int sendData(char *data)/*{{{*/
{
    int len=strlen(data);

    if(SD.connected){
        if(len){
            DBGL("Sending data '%s'",data);
            if(data[len-1]=='\n'){
                fprintf(SD.socketfp,"%s",data);
            }else{
                fprintf(SD.socketfp,"%s\n",data);
            }
        }
    }
    return len;
}/*}}}*/
static int request(char *cmd)/*{{{*/
{
    int numlines=0;
    size_t len=0;

    len=sendData(cmd);
    if(len==strlen(cmd)){
        numlines=rcvData();
    }
    return numlines;
}/*}}}*/
static int rcvData(void)/*{{{*/
{
    char *seperator;
    char *start;
    int numlines=0;
    char *responselineStart;
    int rls;

    responselineStart=configValue("dvbbanner");
    rls=strlen(responselineStart);
    if(SD.connected){
        while(!feof(SD.socketfp)){
            if(fgets(SD.line,MAX_LINE_LENGTH,SD.socketfp)){
                if(strncmp(SD.line,responselineStart,rls-1)==0){
                    /* last line */
                    chomp(SD.line);
                    seperator=strchr(SD.line+rls,'/');
                    if(seperator){
                        *seperator=0;
                        start=SD.line+rls;
                        SD.ver=strdup(start);
                        start=seperator+1;
                        seperator=strchr(seperator+1,' ');
                        if(seperator)
                        {
                            *seperator=0;
                            SD.errmsg=strdup(seperator+1);
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
    }
    return numlines;
}/*}}}*/
static void addLineToBuffer()/*{{{*/
{
    int cn;
    int bufflen;

    bufflen=strlen(SD.data);
    cn=bufflen+strlen(SD.line);
    if(cn<RCV_BUFFER_LENGTH){
        sprintf(SD.data+bufflen,"%s\n",SD.line);
    }else{
        WARN("buffer is undersized");
        DBG(SD.line);
    }
}/*}}}*/
char * lsservices(int adaptornum)/*{{{*/
{
    char *services=NULL;
    int nl;

    setupConnect(adaptornum);
    if(SD.authenticated){
        sprintf(SD.line,"%s","lsservices");
        nl=request(SD.line);
        if(nl>0){
            services=strdup(SD.data);
        }
    }
    closeConnect();
    return services;
}/*}}}*/
