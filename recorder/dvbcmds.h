/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbcmds.h
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Monday  7 March 2016, 04:38:36
 * Last Modified: Saturday 15 October 2016, 10:34:34
 */

#ifndef DVBCMDS_H
#define DVBCMDS_H
#include "dvbctrl.h"
#include "defs.h"
#include "macros.h"
#include "tools.h"
#include "sql.c"
#endif

/*
extern char * dvbcommand(char *cmd,int adaptornum);
extern struct ColonParse *parseColon(char *line);
*/

char * lsservices(int adaptornum);
char * lsmuxes(int adaptornum);
char * lssfs(int adaptornum);
char * getsf(int adaptornum,int filternum);
char * getsfmrl(int adaptornum,char *sfname);
int addsf(int adaptornum,int filternum);
int setsf(int adaptornum,int filternum,char *cname);
int getsfmux(int adaptornum);
int muxforchannel(char *channel);
int selectlcn(int adaptornum,int channelnum);
int findfreefilter(struct AdaptorStatus *AS);
int safeToRecord(int adaptornum,char *channel);
struct AdaptorStatus *adaptorStatus(int adaptornum);
struct FilterStatus **newFilterStatusArray(int numfilters);
void fillFilterStatus(int adaptornum,struct FilterStatus *FS,int num);
void freeAdaptorStatus(struct AdaptorStatus *AS);
void freeFilterStatus(struct FilterStatus *FI);
void logAdaptorStatus(struct AdaptorStatus *AS);
void logFilterStatus(struct FilterStatus *FS);
int streamNewProgram(char *fn, struct Program *CP);
char *getmrl(int adaptornum,int filternum);
int findFilterForFile(int adaptornum,char *fn);
