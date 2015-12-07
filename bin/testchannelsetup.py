#!/usr/bin/env python2
import logging
import logging.handlers
import time
import os
import sys
import signal
import daemon
import lockfile

# setup the cristel lib directory
cristel_lib_dir = os.path.realpath(os.path.join(os.path.dirname(sys.argv[0]), '../lib/python'))
sys.path.append(cristel_lib_dir)

from startstopdvbstreamer import (
        readadapters,
        startdvbstreamer,
        stopdvbstreamer,
        )
from dvbsession import DvbSession
from scheduledb import ScheduleDB
from schedule import Schedule

sfn="/home/chris/.epgdb/cristel.db"

sourcemove=0
muxmove=0
lcnmove=0
nolcn=0
unchanged=0

log=logging.getLogger("cristel")

if __name__ == '__main__':
    log.setLevel(logging.DEBUG)
    handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
    log.addHandler(handler)
    prog=os.path.basename(__file__)
    appdir=os.path.expanduser("~/.epgdb")
    sdbfn=os.path.join(appdir,"cristel.db")
    adapters=readadapters(appdir)
    startdvbstreamer(adapters)
    (ip,na,username,password) = adapters[0]
    s=DvbSession(host="%s" % ip, adaptor=na, username="%s" % username, password="%s" % password, logg=log)
    sdb=ScheduleDB(sdbfn,log)
    lcnames=s.logicalnames()
    services=s.lsservices()
    for svc in services:
        updatechannel=0
        si=s.serviceinfo(svc)
        row=sdb.getname(svc)
        cn=0
        try:
            cn=len(row)
        except:
            pass
        if cn>0:
            if si["ID"] == "233a.4083.4440":
                log.error("martian channel is: %s at %s, %s" % (si["Name"],si["ID"],si["Multiplex UID"]))
            if si["ID"] != row[0]:
                log.info("channel %s has moved source from %s to %s" % (svc,row[0],si["ID"]))
                # sch.updatechannel(si["ID"],si["Name"],si["Multiplex UID"])
                sourcemove+=1
                updatechannel=1
            elif int(si["Multiplex UID"]) != int(row[6]):
                log.info("channel %s has moved mux from %d to %d" % (svc,int(row[6]),int(si["Multiplex UID"])))
                muxmove+=1
                updatechannel=1
            elif (svc in lcnames) and (int(row[5]) != int(lcnames[si["Name"]])):
                log.info("channel %s has moved lcn from %d to %d." % (si["Name"],int(row[5]),int(lcnames[si["Name"]])))
                lcnmove+=1
                updatechannel=1
            elif svc not in lcnames:
                log.warning("Channel " + svc + " not in logical number list.")
                nolcn+=1
                updatechannel=1
            else:
                log.debug("channel " + si["Name"] + " unchanged.")
                unchanged+=1
            if updatechannel==1:
                log.info("updating channel: " + svc)
                if svc in lcnames:
                    lcn=lcnames[svc]
                else:
                    lcn=0
                    log.warning("channel " + svc + " has not got a logical id.")
                log.info("updating channel %s: source: %s, mux: %s, lcn: %s" % (svc,si["ID"],si["Multiplex UID"],lcn))
                sdb.updatelchannel(si["ID"],svc,si["Multiplex UID"],lcn)
        else:
            if svc in lcnames:
                lcn=lcnames[svc]
            else:
                lcn=0
                log.warning("New channel " + svc + " has not got a logical id.")
            log.info("New channel %s, logical id: %d, source: %s, mux: %d is not in database. Adding." % (svc,int(lcn),si["ID"],int(si["Multiplex UID"])))
            sdb.newlchan(si["ID"],svc,si["Multiplex UID"],lcn)
    log.info("mux move: %d" % muxmove)
    log.info("source move: %d" % sourcemove)
    log.info("lcn move: %d" % lcnmove)
    log.info("no lcn: %d" % nolcn)
    log.info("unchanged: %d" % unchanged)
