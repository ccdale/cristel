/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbctrl.c
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Sunday 27 July 2014, 06:07:48
 * Last Modified: Sunday 27 July 2014, 16:23:23
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

#define MAX_LINE_LENGTH 256
#define RCV_BUFFER_LENGTH 8192

static const char responselineStart[] = "DVBStreamer/";
static char line[MAX_LINE_LENGTH];
static char rcvbuf[RCV_BUFFER_LENGTH];
static int connected=0;
static int authenticated=0;

static int dvbc_connect(int adapternum)/*{{{*/
{
    socklen_t address_len;
    struct sockaddr_storage address;
    struct addrinfo *addrinfo, hints;
    char portnumber[10];    
    int socketfd = -1;
    FILE *socketfp;
    char *ver;
    int errno;
    int *errnop=&errno;
    char *errmsg;
    char *username;
    char *password;
    char *host;

    host=configValue("dvbhost");
    username=configValue("dvbuser");
    password=configValue("dvbpass");

    sprintf(portnumber, "%d", REMOTEINTERFACE_PORT + adapternum);
    memset((void *)&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG;
    if ((getaddrinfo(host, portnumber, &hints, &addrinfo) != 0) || (addrinfo == NULL))
    {
        CCAC("Failed to get address");
        return 1;
    }
    if (addrinfo->ai_addrlen > sizeof(struct sockaddr_storage))
    {
        CCAC("Failed to parse address");
        freeaddrinfo(addrinfo);
        return 1;
    }
    address_len = addrinfo->ai_addrlen;
    memcpy(&address, addrinfo->ai_addr, addrinfo->ai_addrlen);
    freeaddrinfo(addrinfo);
    socketfd = socket(address.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (socketfd < 0)
    {
        CCAC("Failed to create socket.");
        return 1;
    }
    if (connect(socketfd, (const struct sockaddr *) &address, address_len))
    {
        CCAC("Failed to connect to host %s port %d", host, REMOTEINTERFACE_PORT + adapternum);
        return 1;
    }
    CCAL("Socket connected to host %s port %d", host, REMOTEINTERFACE_PORT + adapternum);
    socketfp = fdopen(socketfd, "r+");
    receiveData(socketfp, &ver, errnop, &errmsg);
    /*
    if (!fgets(line , MAX_LINE_LENGTH, socketfp))
    {
        CCAC("No ready line received from server.");
        fclose(socketfp);
        return 1;
    }
    ProcessResponseLine(line, &ver, &errno, &errmsg);
    */
    if (errno != 0)
    {
        CCAC("%s", errmsg);
        fclose(socketfp);
        return errno;
    }
    connected=1;
    return 0;
}/*}}}*/
static int Authenticate(FILE *socketfp, char *username, char *password)/*{{{*/
{
    char * ver;
    int errno;
    char *errmsg;
    sprintf(line, "auth %s %s", username, password);
    SendCommand(socketfp, line, &ver, &errno, &errmsg);
    authenticated=(errno==0);
    return authenticated;
}/*}}}*/
static int sendData(FILE *socketfp, char *data)/*{{{*/
{
    int len=strlen(data);

    if(connected){
        if(len){
            DBGL("Sending data '%s'",data);
            if(data[len-1]=='\n'){
                fprintf(socketfp,"%s",data);
            }else{
                fprintf(socketfp,"%s\n",data);
            }
        }
    }
    return len;
}/*}}}*/
static int request(FILE *socketfp, char *cmd, char **ver, int *errno, char **errmsg)/*{{{*/
{
    int numlines=0;
    int len=0;

    len=sendData(socketfp,cmd);
    if(len==strlen(cmd)){
        numlines=receiveData(socketfp,&ver,&errno,&errmsg);
    }
    return numlines;
}/*}}}*/
static int receiveData(FILE *socketfp, char **ver, int* errno, char **errmsg)/*{{{*/
{
    char *seperator;
    char *start;
    int numlines=0;

    if(connected){
        while(!feof(socketfp)){
            if(fgets(line,MAX_LINE_LENGTH,socketfp)){
                if(strncmp(line,responselineStart,sizeof(responselineStart)-1)==0){
                    /* last line */
                    chomp(line);
                    seperator=strchr(line+sizeof(responselineStart),'/');
                    if(seperator){
                        *seperator=0;
                        start=line+sizeof(responselineStart);
                        *ver=strdup(start);
                        start=seperator+1;
                        seperator=strchr(separator+1," ");
                        if(seperator)
                        {
                            *seperator=0;
                            *errmsg=strdup(seperator+1);
                        }
                        *errno=atoi(start);
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
static void addLineToBuffer(void)/*{{{*/
{
    int cn;
    int bufflen;

    bufflen=strlen(rcvbuf);
    cn=bufflen+strlen(line);
    if(cn<RCV_BUFFER_LENGTH){
        sprintf(rcvbuf+bufflen,"%s\n",line);
    }else{
        CCAC("buffer is undersized");
        DBG(line);
    }
}/*}}}*/
