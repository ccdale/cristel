/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbcmds.c
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Monday  7 March 2016, 04:40:22
 * Last Modified: Sunday 18 September 2016, 09:18:19
 */

#include "dvbcmds.h"

/* direct wrappers around dvbstreamer commands */
char * lsservices(int adaptornum)/*{{{*/
{
    char *services=NULL;
    services=dvbcommand("lsservices",adaptornum);
    return services;
}/*}}}*/
char * lsmuxes(int adaptornum)/*{{{*/
{
    char *muxes=NULL;
    muxes=dvbcommand("lsmuxes",adaptornum);
    return muxes;
}/*}}}*/
char * lssfs(int adaptornum)/*{{{*/
{
    char *filters=NULL;
    filters=dvbcommand("lssfs",adaptornum);
    return filters;
}/*}}}*/
char * getsf(int adaptornum,int filternum)/*{{{*/
{
    char *channel=NULL;
    char *cmd;
    char *line;
    int len;
    struct ColonParse *CP;

    cmd=xmalloc(MAX_MSG_LEN);
    if(filternum==0){
        len=snprintf(cmd,MAX_MSG_LEN,"getsf '<Primary>'");
    }else{
        len=snprintf(cmd,MAX_MSG_LEN,"getsf dvb%d",filternum);
    }
    line=dvbcommand(cmd,adaptornum);
    len=strlen(line);
    if(len){
        CP=parseColon(line);
        channel=strdup(CP->val);
        if(CP->tmp){
            free(CP->tmp);
        }
        free(CP);
    }
    free(line);
    free(cmd);
    return channel;
}/*}}}*/
char * getsfmrl(int adaptornum,char *sfname)/*{{{*/
{
    char *cmd=NULL;
    char *mrl;
    int len;

    len=snprintf(cmd,0,"getsfmrl %s",sfname);
    len++;
    cmd=xmalloc(len);
    len=snprintf(cmd,len,"getsfmrl %s",sfname);
    mrl=dvbcommand(cmd,adaptornum);
    free(cmd);
    return mrl;
}/*}}}*/
int addsf(int adaptornum,int filternum)/*{{{*/
{
    char *cmd=NULL;
    char *junk;
    int len;

    junk=xmalloc(MAX_MSG_LEN);
    len=snprintf(cmd,0,"addsf dvb%d null://",filternum);
    len++;
    cmd=xmalloc(len);
    len=snprintf(cmd,len,"addsf dvb%d null://",filternum);
    junk=dvbcommand(cmd,adaptornum);
    free(cmd);
    free(junk);
    return filternum;
}/*}}}*/

/* helper commands */
int getsfmux(int adaptornum)/*{{{*/
{
    char *channel=NULL;
    int mux=0;
    
    channel=getsf(adaptornum,0);
    if(channel){
        mux=muxforchannel(channel);
    }
    return mux;
}/*}}}*/
int muxforchannel(char *channel)/*{{{*/
{
    int mux;
    struct ServiceInfo *SI;

    SI=getServiceInfo(channel);
    mux=SI->mux;
    freeServiceInfo(SI);
    return mux;
}/*}}}*/
int selectlcn(int adaptornum,int channelnum)/*{{{*/
{
    char *line;
    char *cmd;
    int len;
    struct ColonParse *CP;
    int ret=0;

    cmd=xmalloc(MAX_MSG_LEN);
    len=snprintf(cmd,MAX_MSG_LEN,"selectlcn %d",channelnum);
    line=dvbcommand(cmd,adaptornum);
    len=strlen(line);
    if(len){
        CP=parseColon(line);
        INFO("selectlcn set channel %d (%s) on Primary filter of adaptor %d",channelnum,CP->val,adaptornum);
        if(CP->tmp){
            free(CP->tmp);
        }
        free(CP);
        ret=channelnum;
    }
    free(line);
    free(cmd);
    return ret;
}/*}}}*/
int findfreefilter(struct AdaptorStatus *AS)/*{{{*/
{
    int ff=-1;
    int cn;
    struct FilterStatus **FS;

    if(AS->recording==AS->numfilters){
        ff=AS->numfilters+1;
        ff=addsf(AS->adaptornum,ff);
    }else{
        FS=AS->FS;
        for(cn=0;cn<AS->numfilters;cn++){
            if(strcmp(FS[cn]->mrl,"null://")==0){
                ff=cn;
                break;
            }
        }
    }
    return ff;
}/*}}}*/
int safeToRecord(int adaptornum,char *channel)/*{{{*/
{
    int ret=-1;
    int mux;
    struct AdaptorStatus *AS;

    AS=adaptorStatus(adaptornum);
    if(AS->recording==0){
        ret=0;
    }else{
        mux=muxforchannel(channel);
        if(AS->mux==mux){
            ret=findfreefilter(AS);
        }
    }
    freeAdaptorStatus(AS);
    return ret;
}/*}}}*/
struct AdaptorStatus *adaptorStatus(int adaptornum)/*{{{*/
{
    struct AdaptorStatus *AI;
    char *lines;
    int cn;

    AI=xmalloc(sizeof(struct AdaptorStatus));
    AI->adaptornum=adaptornum;
    lines=lssfs(adaptornum);
    DEBUG("lssfs: %s",lines);
    AI->numfilters=numLines(lines);
    DEBUG("num filters (lines): %d",AI->numfilters);
    AI->recording=0;
    AI->FS=newFilterStatusArray(AI->numfilters);
    AI->mux=0;
    for(cn=0; cn<AI->numfilters; cn++){
        fillFilterStatus(adaptornum,AI->FS[cn],cn);
        if(strcmp("null://",AI->FS[cn]->mrl)!=0){
            AI->recording++;
        }
        if(AI->mux==0){
            if(AI->FS[cn]->channel){
                AI->mux=muxforchannel(AI->FS[cn]->channel);
            }
        }
    }
    return AI;
}/*}}}*/
struct FilterStatus **newFilterStatusArray(int numfilters)/*{{{*/
{
    struct FilterStatus **FI;
    int cn;

    FI=xmalloc(sizeof(struct FilterStatus *) * numfilters);
    for(cn=0; cn<numfilters; cn++){
        FI[cn]=xmalloc(sizeof(struct FilterStatus));
        FI[cn]->num=cn;
        FI[cn]->name=NULL;
        FI[cn]->channel=NULL;
        FI[cn]->mrl=NULL;
    }
    return FI;
}/*}}}*/
void fillFilterStatus(int adaptornum,struct FilterStatus *FS,int num)/*{{{*/
{
    char *name=NULL;
    int len;

    if(num==0){
        len=snprintf(name,0,"<Primary>");
        len++;
        name=xmalloc(len);
        len=snprintf(name,len,"<Primary>");
    }else{
        len=snprintf(name,0,"dvb%d",num);
        len++;
        name=xmalloc(len);
        len=snprintf(name,len,"dvb%d",num);
    }
    FS->name=name;
    FS->num=num;
    FS->channel=getsf(adaptornum,FS->num);
    FS->mrl=getsfmrl(adaptornum,FS->name);
}/*}}}*/
void freeAdaptorStatus(struct AdaptorStatus *AStatus)/*{{{*/
{
    int cn;

    for(cn=0; cn<AStatus->numfilters; cn++){
        freeFilterStatus(AStatus->FS[cn]);
        free(AStatus->FS[cn]);
    }
    free(AStatus->FS);
    free(AStatus);
}/*}}}*/
void freeFilterStatus(struct FilterStatus *FI)/*{{{*/
{
    if(FI->name){
        free(FI->name);
    }
    if(FI->mrl){
        free(FI-mrl);
    }
    if(FI->channel){
        free(FI->channel);
    }
}/*}}}*/
