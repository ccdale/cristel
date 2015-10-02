#!/usr/bin/env python2
"""run after freeview has moved stuff around"""
import logging
import logging.handlers
import time
import os.path as path
import sys

# setup the cristel lib directory
cristel_lib_dir = path.realpath(path.join(path.dirname(sys.argv[0]), '../lib/python'))
sys.path.append(cristel_lib_dir)

import startstopdvbstreamer as dvb
from dvbsession import DvbSession
from scheduledb import ScheduleDB

log=logging.getLogger("cristel")
log.setLevel(logging.INFO)
handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
log.addHandler(handler)

# see https://ukfree.tv/article/1107051920/Freeview_modes_a_simplified_explanation_
modes={
        'dtg3':"8MHz 2/3 NONE QAM64 8k 1/32 NONE",
        'dtg8':"8MHz 3/4 NONE QAM64 8k 1/32 NONE",
        'dtg6':"8MHz 2/3 NONE QAM256 32KE 1/128 NONE"
        }
# see https://ukfree.tv/transmitters/tv/Sandy_Heath
freqs={
        # '474200000':modes['dtg6'], # hd
        # '562000000':modes['dtg6'], # hd
        # '578000000':modes['dtg6'], # hd
        '522000000':modes['dtg3'], 
        '498000000':modes['dtg3'],
        '714000000':modes['dtg8'], 
        '722000000':modes['dtg8'], 
        '690000000':modes['dtg8'] 
        }

sfn="/home/chris/.epgdb/cristel.db"
muxmove=0
sourcemove=0
newchan=0
unchanged=0

if not dvb.isdvbstreamerrunning():
    log.debug("starting dvbstreamer")
    dvb.startdvbstreamer(1,"tvc","tvc")
    log.debug("giving dvbstreamer 5 seconds to start")
    time.sleep(5)

if dvb.isdvbstreamerrunning():
    sch=ScheduleDB(sfn,log)
    s=DvbSession("127.0.0.1",0,"tvc","tvc",log)
    for freq in freqs:
        s.scannet("T " + freq + " " + freqs[freq])

    log.info("updating channels")
    muxes=s.lsmuxes()
    for mux in muxes:
        log.debug("reading services for mux: " + mux)
        services=s.servicesformux(mux)
        cn=len(services)
        log.info("mux: %s has %s services" % (mux,cn))
        for svc in services:
            si=s.serviceinfo(svc)
            row=sch.getname(svc)
            cn=0
            try:
                cn=len(row)
            except:
                pass

            if cn > 0:
                log.debug("Channel " + si["Name"] + " already exists")
                if si["ID"] != row[0]:
                    log.info("channel " + si["Name"] + " has moved source, updating db")
                    sch.updatechannel(si["ID"],si["Name"],si["Multiplex UID"])
                    sourcemove+=1
                elif si["Multiplex UID"] != row[6]:
                    log.info("channel " + si["Name"] + " has moved mux, updating db")
                    sch.updatechannel(si["ID"],si["Name"],si["Multiplex UID"])
                    muxmove+=1
                else:
                    unchanged+=1
            else:
                log.info("inserting new Channel: " + si["Name"])
                sch.newchan(si["ID"],si["Name"],si["Multiplex UID"])
                newchan+=1

    log.info("%s new channels" % newchan)
    log.info("%s moved source" % sourcemove)
    log.info("%s moved mux" % muxmove)
    log.info("%s unchanged" % unchanged)

else:
    log.error("dvbstreamer failed to start")
