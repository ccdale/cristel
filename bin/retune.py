#!/usr/bin/env python2
"""run after freeview has moved stuff around"""
import logging
import logging.handlers
import time
import os.path as path
import sys
from optparse import OptionParser

# setup the cristel lib directory
cristel_lib_dir = path.realpath(path.join(path.dirname(sys.argv[0]), '../lib/python'))
sys.path.append(cristel_lib_dir)

from startstopdvbstreamer import *
from dvbsession import DvbSession
from scheduledb import ScheduleDB

log=logging.getLogger("cristel")
log.setLevel(logging.INFO)
#log.setLevel(logging.DEBUG)
handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
log.addHandler(handler)

def output(msg):
  log.info(msg)
  print(msg)

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


prog=os.path.basename(__file__)
parser = OptionParser(
          'usage: %prog [-n]')
parser.add_option('-n', '--noscan', action="store_true", dest='noscan', help='don\'t scan, just check channels')
(options, args) = parser.parse_args()

if not isdvbstreamerrunning():
    appdir=os.path.expanduser("~/.epgdb")
    adapters=readadapters(appdir)
    output("starting dvbstreamer")
    startdvbstreamer(adapters)
    log.debug("giving dvbstreamer 5 seconds to start")
    time.sleep(5)

if isdvbstreamerrunning():
    sch=ScheduleDB(sfn,log)
    # these chans cause probs, so delete and reinsert them
    sql="delete from Channels where name like 'Children%'"
    sch.dosql(sql)
    sql="delete from Channels where name='Travel Channel'"
    sch.dosql(sql)
    s=DvbSession("127.0.0.1",0,"tvc","tvc",log)
    if not options.noscan:
      for freq in freqs:
        output("scanning %s %s" % (freq,freqs[freq]))
        s.scannet("T " + freq + " " + freqs[freq])

    lcns=s.logicalnames()
    output("updating channels")
    muxes=s.lsmuxes()
    for mux in muxes:
        log.debug("reading services for mux: " + mux)
        services=s.servicesformux(mux)
        cn=len(services)
        output("mux: %s has %s services" % (mux,cn))
        for svc in services:
            si=s.serviceinfo(svc)
            row=sch.getname(svc)
            cn=0
            try:
                cn=len(row)
            except:
                pass

            if si["Name"] in lcns:
                lcn=lcns[si["Name"]]
            else:
                lcn=0

            if cn > 0:
                log.debug("Channel " + si["Name"] + " already exists")
                if si["ID"] != row[0]:
                    output("channel " + si["Name"] + " has moved source, updating db")
                    sch.updatelchannel(si["ID"],si["Name"],si["Multiplex UID"],lcn)
                    sourcemove+=1
                elif si["Multiplex UID"] != str(row[6]):
                    output("channel " + si["Name"] + " has moved mux, updating db")
                    output("previous: %d" % row[6])
                    output("now: %s" % si["Multiplex UID"])
                    sch.updatelchannel(si["ID"],si["Name"],si["Multiplex UID"],lcn)
                    muxmove+=1
                else:
                    unchanged+=1
            else:
                output("inserting new Channel: " + si["Name"])
                sch.newchan(si["ID"],si["Name"],si["Multiplex UID"])
                sch.updatelchannel(si["ID"],si["Name"],si["Multiplex UID"],lcn)
                newchan+=1

    output("%s new channels" % newchan)
    output("%s moved source" % sourcemove)
    output("%s moved mux" % muxmove)
    output("%s unchanged" % unchanged)

else:
    log.error("dvbstreamer failed to start")
