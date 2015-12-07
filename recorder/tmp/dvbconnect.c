/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbconnect.c
 *
 * Started: Saturday  9 August 2014, 15:54:39
 * Last Modified: Sunday 10 August 2014, 17:59:35
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

#include "dvbconnect.h"

static FILE *socketfp=NULL;
static const char responselineStart[] = "DVBStreamer/";
char line[MAX_LINE_LENGTH];
static char rcvbuf[RCV_BUFFER_LENGTH];
static int connected=0;
static int authenticated=0;
static struct DvbcMsg {
    int errornumber;
    char *ver;
    char *errmsg;
} dvbcmsg;

int dvbc_connect(int adapternum)/*{{{*/
{
    socklen_t address_len;
    struct sockaddr_storage address;
    struct addrinfo *addrinfo, hints;
    char portnumber[10];    
    int socketfd = -1;
    char *host;

    if(!connected){
        host=configValue("dvbhost");

        clear_dvbcmsg();

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
            CCAC("errno: %d, %s",errno,strerror(errno));
            CCAC("Failed to connect to host %s port %d", host, REMOTEINTERFACE_PORT + adapternum);
            return 1;
        }
        CCAL("Socket connected to host %s port %d", host, REMOTEINTERFACE_PORT + adapternum);
        socketfp = fdopen(socketfd, "r+");
        receiveData();
        if (dvbcmsg.errornumber != 0)
        {
            CCAC("%s", dvbcmsg.errmsg);
            fclose(socketfp);
            connected=0;
            return dvbcmsg.errornumber;
        }else{
            connected=1;
        }
    }
    if(!authenticated){
        if(Authenticate()){
            clear_dvbcmsg();
            DBGL("authenticated: %d",authenticated);
        }else{
            CCAC("Failed to authenticate: %s",dvbcmsg.errmsg);
        }
    }
    return authenticated;
}/*}}}*/
void dvbc_disconnect(void)/*{{{*/
{
    if(socketfp){
        fclose(socketfp);
        socketfp=NULL;
        connected=0;
        authenticated=0;
    }
}/*}}}*/
int Authenticate()/*{{{*/
{
    char *username;
    char *password;

    username=configValue("dvbuser");
    password=configValue("dvbpass");
    sprintf(line, "auth %s %s", username, password);
    request(line);
    authenticated=(dvbcmsg.errornumber==0);
    return authenticated;
}/*}}}*/
int sendData(char *data)/*{{{*/
{
    int len=strlen(data);

    if(connected){
        if(len){
            DBGL("Sending data '%s'",data);
            if(data[len-1]=='\n'){
                DBG("Not adding new line");
                fprintf(socketfp,"%s",data);
            }else{
                DBG("adding new line");
                fprintf(socketfp,"%s\n",data);
            }
        }
    }
    return len;
}/*}}}*/
int request(char *cmd)/*{{{*/
{
    int numlines=0;
    unsigned int len=0;

    len=sendData(cmd);
    if(len==strlen(cmd)){
        numlines=receiveData();
    }else{
        DBGL("sendData sent %d bytes, cmd is %d bytes",len,(int)strlen(cmd));
    }
    return numlines;
}/*}}}*/
int receiveData(void)/*{{{*/
{
    char *seperator;
    char *start;
    int numlines=0;

    if(connected){
        clear_dvbcmsg();
        emptyBuffer();
        /* while(!feof(socketfp)){ */
        while(1){
            DBG("Reading socket");
            if(fgets(line,MAX_LINE_LENGTH,socketfp)){
                DBGL(">> %s",line);
                if(strncmp(line,responselineStart,sizeof(responselineStart)-1)==0){
                    /* last line */
                    chomp(line);
                    seperator=strchr(line+sizeof(responselineStart),'/');
                    if(seperator){
                        *seperator=0;
                        start=line+sizeof(responselineStart)-1;
                        dvbcmsg.ver=strdup(start);
                        start=seperator+1;
                        seperator=strchr(seperator+1,' ');
                        if(seperator)
                        {
                            *seperator=0;
                            dvbcmsg.errmsg=strdup(seperator+1);
                        }
                        dvbcmsg.errornumber=atoi(start);
                        DBGL("ver: %s",dvbcmsg.ver);
                        DBGL("errornumber: %d",dvbcmsg.errornumber);
                        DBGL("errmsg: %s",dvbcmsg.errmsg);
                        /*
                        numlines++;
                        */
                    }
                    /* grab any junk that may be left in the buffer */
                    /*
                    DBG("grabbing junk");
                    if(fgets(line,MAX_LINE_LENGTH,socketfp)){
                        DBGL("Extra junk: %s",line);
                    }
                    */
                    break;
                }else{
                    /* data lines */
                    addLineToBuffer();
                    numlines++;
                }
            }else{
                DBG("Nothing received from fgets");
                break;
            }
        }
    }
    return numlines;
}/*}}}*/
void addLineToBuffer(void)/*{{{*/
{
    int cn;
    int bufflen;
    char *xline;

    bufflen=strlen(rcvbuf);
    xline=trim(line);
    cn=bufflen+strlen(xline);
    if(cn<RCV_BUFFER_LENGTH){
        sprintf(rcvbuf+bufflen,"%s\n",xline);
    }else{
        CCAC("buffer is undersized");
        DBG(line);
    }
}/*}}}*/
void emptyBuffer(void) /*{{{*/
{
    sprintf(rcvbuf,"%s","");
} /*}}}*/
void clear_dvbcmsg(void)/*{{{*/
{
    dvbcmsg.errornumber=0;
    if(dvbcmsg.ver){
        free(dvbcmsg.ver);
        dvbcmsg.ver=NULL;
    }
    if(dvbcmsg.errmsg){
        free(dvbcmsg.errmsg);
        dvbcmsg.errmsg=NULL;
    }
}/*}}}*/
