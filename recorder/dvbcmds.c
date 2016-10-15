/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbcmds.c
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Monday  7 March 2016, 04:40:22
 * Last Modified: Saturday 15 October 2016, 11:29:11
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
    char *cmd=NULL;
    char *line=NULL;
    struct ColonParse *CP;
    int nl;

    if(filternum==0){
        cmd=fitstring("current");
    }else{
        cmd=fitstring("getsf dvb%d",filternum);
    }
    if(cmd){
        nl=dvbcmd(cmd,adaptornum,&line);
        if(nl>0){
            CP=parseColon(line);
            if(CP->val){
                channel=strdup(CP->val);
            }
            if(CP->tmp){
                free(CP->tmp);
            }
            free(CP);
        }
        free(cmd);
        free(line);
    }
    return channel;
}/*}}}*/
char * getsfmrl(int adaptornum,char *sfname)/*{{{*/
{
    char *cmd=NULL;
    char *mrl;

    cmd=fitstring("getsfmrl %s",sfname);
    mrl=dvbcommand(cmd,adaptornum);
    free(cmd);
    return mrl;
}/*}}}*/
int addsf(int adaptornum,int filternum)/*{{{*/
{
    char *cmd=NULL;
    char *junk;

    cmd=fitstring("addsf dvb%d null://",filternum);
    junk=xmalloc(MAX_MSG_LEN);
    junk=dvbcommand(cmd,adaptornum);
    free(cmd);
    free(junk);
    return filternum;
}/*}}}*/
int setsf(int adaptornum,int filternum,char *cname)/* {{{1 */
{
    char *cmd=NULL;
    char *output=NULL;
    int ret=1;
    int nl;

    if(filternum==0){
        cmd=fitstring("select '%s'",cname);
    }else{
        cmd=fitstring("setsf dvb%d '%s'",filternum,cname);
    }
    if(cmd){
        nl=dvbcmd(cmd,adaptornum,&output);
        if(nl>0){
            DEBUG("setsf: %d lines",nl);
            DEBUG("setsf: output: %s",output);
            free(output);
            ret=0;
        }
        free(cmd);
    }else{
        WARN("setsf: failed to allocate cmd string for filter %d and channel %s",filternum,cname);
    }
    return ret;
}/* }}} */
int setsfmrl(int adaptornum, int filternum, char *mrl)/* {{{1 */
{
    char *cmd=NULL;
    char *output=NULL;
    int ret=1;
    int nl;

    if(filternum==0){
        cmd=fitstring("setmrl '%s'",mrl);
    }else{
        cmd=fitstring("setsfmrl dvb%d '%s'",filternum,mrl);
    }
    if(cmd){
        nl=dvbcmd(cmd,adaptornum,&output);
        if(nl>0){
            DEBUG("setsfmrl: %d lines",nl);
            DEBUG("setsfmrl: output: %s",output);
            if(output){
                free(output);
            }
            ret=0;
        }
        free(cmd);
    }else{
        WARN("setsfmrl: failed to allocate cmd string for filter %d and mrl %s",filternum,mrl);
    }
    return ret;
}/* }}} */

/* helper commands */
char *getmrl(int adaptornum,int filternum)/* {{{1 */
{
    char *cmd=NULL;
    char *mrl;

    if(filternum==0){
        cmd=fitstring("%s","getmrl");
    }else{
        cmd=fitstring("getsfmrl dvb%d",filternum);
    }
    mrl=dvbcommand(cmd,adaptornum);
    free(cmd);
    return mrl;
}/* }}} */
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

    cmd=fitstring("selectlcn %d",channelnum);
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
int findfreefilter(struct AdaptorStatus *AX)/*{{{*/
{
    int ff=-1;
    int cn;
    struct FilterStatus **FS;

    if(AX->recording==AX->numfilters){
        ff=AX->numfilters+1;
        ff=addsf(AX->adaptornum,ff);
    }else{
        FS=AX->FS;
        for(cn=0;cn<AX->numfilters;cn++){
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
    logAdaptorStatus(AS);
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
    free(lines);
    AI->recording=0;
    AI->FS=newFilterStatusArray(AI->numfilters);
    AI->mux=0;
    for(cn=0; cn<AI->numfilters; cn++){
        fillFilterStatus(adaptornum,AI->FS[cn],cn);
        if(strncmp("null:",AI->FS[cn]->mrl,5)!=0){
            INFO("adaptor %d is currently recording on filter %s",adaptornum,AI->FS[cn]->name);
            INFO("mrl: %s",AI->FS[cn]->mrl);
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

    if(num==0){
        name=fitstring("%s","<Primary>");
    }else{
        name=fitstring("dvb%d",num);
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
        free(FI->mrl);
    }
    if(FI->channel){
        free(FI->channel);
    }
}/*}}}*/
void logAdaptorStatus(struct AdaptorStatus *AS)/*{{{*/
{
    int cn;

    INFO("Adaptor Status: Adaptor: %d",AS->adaptornum);
    INFO("Numfilters: %d",AS->numfilters);
    INFO("recording: %d",AS->recording);
    INFO("mux: %d",AS->mux);

    for(cn=0; cn<AS->numfilters; cn++){
        logFilterStatus(AS->FS[cn]);
    }
}/*}}}*/
void logFilterStatus(struct FilterStatus *FS)/*{{{*/
{
    INFO("  FS: filter num: %d",FS->num);
    if(FS->name){
        INFO("  FS: name: %s",FS->name);
    }else{
        INFO("  FS: name not set");
    }
    if(FS->channel){
        INFO("  FS: channel: %s",FS->channel);
    }else{
        INFO("  FS: channel not set");
    }
    if(FS->mrl){
        INFO("  FS: mrl: %s",FS->mrl);
    }else{
        INFO("  FS: mrl not set");
    }
}/*}}}*/
int streamNewProgram(char *fn,struct Program *CP)/* {{{1 */
{
    int adaptor;
    int freefilter;
    int c;
    int ret=1;
    char *mrl=NULL;

    c=2;
    for(adaptor=0;adaptor<c;adaptor++){
        if((freefilter=safeToRecord(adaptor,CP->cname))!=-1){
            break;
        }
    }
    if(freefilter==-1){
        WARN("streamNewProgram failed to find free filter on any adaptor");
    }else{
        if((c=setsf(adaptor,freefilter,CP->cname))==0){
            CP->filter=freefilter;
            CP->fn=strdup(fn);
            mrl=fitstring("file://%s",fn);
            if(mrl){
                if((c=setsfmrl(adaptor,freefilter,mrl))==0){
                    sleep(1);
                    CP->fnfz=filesize(fn);
                    if(CP->fnfz>0){
                        ret=0;
                        INFO("streamNewProgram: recording %s to %s",CP->cname,fn);
                        /* DEBUG("streamNewProgram: recording %s to %s",CP->cname,fn); */
                    }
                }else{
                    WARN("streamNewProgram: failed to set mrl on adaptor: %d with filter: %d of: %s",adaptor,freefilter,mrl);
                }
                free(mrl);
            }else{
                WARN("streamNewProgram: failed to allocate memory for mrl");
            }
        }else{
            WARN("streamNewProgram: failed to tune adaptor %d with filter %d to channel %s",adaptor,freefilter,CP->cname);
        }
    }
    return ret;
}/* }}} */
int findFilterForFile(int adaptornum,char *fn)/* {{{1 */
{
    struct AdaptorStatus *AS;
    int ret=-1;
    int x;
    char *cmp;

    AS=adaptorStatus(adaptornum);
    logAdaptorStatus(AS);
    if(AS->recording!=0){
        cmp=fitstring("file://%s",fn);
        for(x=0;x<AS->numfilters;x++){
            DEBUG("findFilterForFile: comparing %s with %s",fn,AS->FS[x]->mrl);
            if(strcmp(cmp,AS->FS[x]->mrl)==0){
                ret=x;
                DEBUG("found %s at filter %d on adaptor %d",fn,x,adaptornum);
                break;
            }
        }
        free(cmp);
    }
    return ret;
}/* }}} */
