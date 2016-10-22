#!/usr/bin/env python2
import logging
import logging.handlers
import time
import os
import sys
import signal
import daemon
import lockfile
import signal

# setup the cristel lib directory
cristel_lib_dir = os.path.realpath(os.path.join(os.path.dirname(sys.argv[0]), '../lib/python'))
sys.path.append(cristel_lib_dir)

# from scheduledb import ScheduleDB
from schedule import Schedule

log=logging.getLogger("cristel")

if __name__ == '__main__':
    left=3600
    togo=3600
    log.setLevel(logging.INFO)
    handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
    log.addHandler(handler)
    prog=os.path.basename(__file__)
    log.debug("starting scheduler class")
    sch=Schedule(log)
    log.debug("updating schedule")
    sch.makeschedule()
    with file("/home/chris/cristel.pid") as f:
      try:
        pid=f.read()
        os.kill(pid,signal.SIGUSR1)
      except:
        log.warn("cristel pid file not readable or not found")
