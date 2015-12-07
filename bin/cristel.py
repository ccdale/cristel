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
# from scheduledb import ScheduleDB
from schedule import Schedule

log=logging.getLogger("cristel")

def nformat(num,width=2,padleft=True,padchar="0"):
    s=str(num)
    cn=len(s)
    while cn<width:
        if padleft:
            s="%s%s" % (padchar,s)
        else:
            s="%s%s" % (s,padchar)
        cn=len(s)
    return s

def timeformat(secs):
    days=0
    hours=0
    mins=0
    if secs>=60:
        mins=int(secs / 60)
        secs=secs % 60
    if mins>=60:
        hours=int(mins / 60)
        mins=mins % 60
    if hours>=24:
        days=int(hours / 24)
        hours=hours % 24
    if days>0:
        op="%d:%d:%d:%d" % (days,nformat(hours),nformat(mins),nformat(secs))
    elif hours>0:
        op="%d:%d:%d" % (hours,nformat(mins),nformat(secs))
    elif mins>0:
        op="%d:%d" % (mins,nformat(secs))
    else:
        op="%d seconds" % secs
    return op

def theLoop(adapters,log):
    log.debug("starting scheduler class")
    sch=Schedule(log)
    while 1:
        log.debug("starting loop")
        log.debug("updating schedule")
        sch.makeschedule()
        row=sch.getnextschedule()
        now=int(time.time())
        left=3600
        togo=3600
        if len(row):
            togo=row["start"] - now
            log.debug("next recording: %s in %s" % (row["title"],timeformat(togo)))
        else:
            log.debug("no recordings scheduled")

        rrows=sch.getcurrentrecordings()
        cn=len(rrows)
        if cn:
            log.debug("%d programmes currently being recorded" % cn)
            nextend=rrows[0]["end"]
            left=nextend - now
            log.debug("%s finishes in %s" % (rrows[0]["title"],timeformat(left)))
        else:
            log.debug("nothing currently recording")
        if togo<left:
            timeleft=togo
        else:
            timeleft=left
        log.debug("sleeping for %s" % timeformat(timeleft))
        signal.alarm(left)
        signal.pause()
        log.debug("ending loop")

def cleanup(signum, stackframe):
    log.debug("cleanup signal caught")
    stopdvbstreamer()
    sys.exit(0)

def loadconfig():
    log.debug("loadconfig")

def reloadconfig(signum, stackframe):
    log.debug("reload config")

def sigalarm(signum, stackframe):
    log.debug("sig alarm")

if __name__ == '__main__':
    log.setLevel(logging.DEBUG)
    handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
    log.addHandler(handler)
    prog=os.path.basename(__file__)
    appdir=os.path.expanduser("~/.epgdb")
    adapters=readadapters(appdir)
    startdvbstreamer(adapters)
    context = daemon.DaemonContext(
        working_directory=appdir,
        umask=0o002,
        pidfile=lockfile.FileLock(os.path.join(appdir,"cristel.pid")),
    )
    context.signal_map = {
        signal.SIGTERM: cleanup,
        signal.SIGHUP: loadconfig,
        signal.SIGUSR1: reloadconfig,
        signal.SIGALRM: sigalarm,
    }
    with context:
        # signal.alarm(30)
        theLoop(adapters,log)
