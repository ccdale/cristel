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

# from scheduledb import ScheduleDB
from schedule import Schedule

log=logging.getLogger("cristel")

if __name__ == '__main__':
    left=3600
    togo=3600
    log.setLevel(logging.DEBUG)
    handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
    log.addHandler(handler)
    prog=os.path.basename(__file__)
    log.debug("starting scheduler class")
    sch=Schedule(log)
    log.debug("updating schedule")

    sch.makeschedule()
    row=sch.getnextschedule()
    if row != None:
        now=int(time.time())
        togo=row["start"] - now
        log.info("next recording: %s in %d seconds" % (row["title"],togo))
    else:
        log.info("no recordings scheduled")

    rrows=sch.getcurrentrecordings()
    cn=len(rrows)
    if cn:
        log.debug("%d programmes currently being recorded" % cn)
        nextend=rrows[0]["end"]
        left=nextend - now
        log.debug("%s finishes in %d seconds" % (rrows[0]["title"],left))
    else:
        log.info("nothing currently recording")
    if togo<left:
        timeleft=togo
    else:
        timeleft=left
