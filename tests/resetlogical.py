#!/usr/bin/env python2
# Copyright (c) 2015 Chris Allison chris.allison@hotmail.com
# 
# This file is part of cristel.
# 
# cristel is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# cristel is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with cristel.  If not, see <http://www.gnu.org/licenses/>.

"""Resets the logical channel number (freeview number) and muxid for each channel"""

import sys
import os
import os.path as path
import logging
import logging.handlers
import sqlite3
import xmltv
import time

# setup the cristel lib directory
cristel_lib_dir = path.realpath(path.join(path.dirname(sys.argv[0]), '../lib/python'))
sys.path.append(cristel_lib_dir)

from scheduledb import ScheduleDB
import startstopdvbstreamer as dvb
from dvbsession import DvbSession

log=logging.getLogger("cristel")
log.setLevel(logging.DEBUG)
handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
log.addHandler(handler)

appdir=os.path.expanduser("~/.epgdb")
scheddb=os.path.join(appdir,"cristel.db")
logchfn=os.path.join(appdir,"logical.channels")

# sdb=ScheduleDB(scheddb,log)
# 
# with open(logchfn) as f:
#     lines=f.readlines()
#     data=()
#     for line in lines:
#         tmp=line.split(":")
#         cn=int(tmp[0].strip())
#         name=tmp[1].strip()
#         t=(cn,name)
#         data+=t,
#     log.debug(data)
#     sdb.updatelogical(data)

adaptors=[('127.0.0.1',0,'tvc','tvc'),('127.0.0.1',1,'tvc','tvc')]
lna=len(adaptors)
(ip,na,username,password) = adaptors[0]
if not dvb.isdvbstreamerrunning():
    log.debug("Starting dvbstreamer")
    dvb.startdvbstreamer(lna,username,password)
    time.sleep(5)
else:
    log.debug("dvbstreamer is running")

if dvb.isdvbstreamerrunning():
    log.debug("starting session")
    s=DvbSession(host="%s" % ip, adaptor=na, username="%s" % username, password="%s" % password, logg=log)
    log.debug("reading logical channel numbers")
    lcn=s.logicalchannels()
    log.debug("closing session")
    s.close()
    tlcn=()
    for cn,name in lcn.iteritems():
        t=(int(cn),name)
        tlcn+=t,

    sdb=ScheduleDB(scheddb,log)
    log.debug("updating logical channel numbers")
    sdb.updatelogical(tlcn)

    log.debug("starting session")
    s=DvbSession(host="%s" % ip, adaptor=na, username="%s" % username, password="%s" % password, logg=log)
    log.debug("reading muxes")
    muxes=s.lsmuxes()
    tsvcs=()
    for mux in muxes:
        log.debug("reading services for mux: %s" % mux)
        svcs=s.servicesformux(mux)
        for svc in svcs:
            t=(mux,svc)
            tsvcs+=t,

    log.debug("updating mux information")
    sdb.updatemuxes(tsvcs)
