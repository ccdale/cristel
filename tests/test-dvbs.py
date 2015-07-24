#!/usr/bin/env python2
"""tests that dvbstreamer can be started and stopped
   and that both adaptors can be used to stream 2 services
   to files at the same time (4 programmes at once)
"""
import os.path as path
import sys
import time
import logging

# setup the cristel lib directory
cristel_lib_dir = path.realpath(path.join(path.dirname(sys.argv[0]), '../lib/python'))
sys.path.append(cristel_lib_dir)

import startstopdvbstreamer as dvb
from dvbsession import DvbSession

logging.basicConfig(level=logging.DEBUG)

def printsfs(sess):
    sfs=sess.get_service_filters()
    str=[]
    for sf in sfs:
        str.append(sf.name)
        str.append(sf.service)
        str.append(sf.mrl)
        logging.debug("Filter: %s" % str)
        str=[]
        # print sf.name
        # print sf.service
        # print sf.mrl

adaptors=[('127.0.0.1',0,'tvc','tvc'),('127.0.0.1',1,'tvc','tvc')]
lna=len(adaptors)
(ip,na,username,password) = adaptors[0]
dvb.startdvbstreamer(lna,username,password)
logging.debug("giving dvbstreamer 5 seconds to start")
time.sleep(5)
if dvb.isdvbstreamerrunning():
    logging.debug("dvbstreamer has started")
    logging.debug("starting sessions")
    s=DvbSession(host="%s" % ip,adaptor=na,username="%s" % username,password="%s" % password)
    xs=DvbSession(host="%s" % ip,adaptor=1,username="%s" % username,password="%s" % password)
    logging.debug("Attempting a recording of bbc2 to test-bbc-2.ts on adaptor 0")
    s.recordprogramme("BBC TWO","/home/chris/test-bbc-2.ts")
    time.sleep(2)
    logging.debug("Attempting a recording of bbc1 to test-bbc-1.ts on adaptor 0")
    s.recordprogramme("BBC ONE East W","/home/chris/test-bbc-1.ts")
    time.sleep(2)
    printsfs(s)
    logging.debug("Attempting a recording of Channel 4 to test-4.ts on adaptor 1")
    xs.recordprogramme("Channel 4","/home/chris/test-4.ts")
    time.sleep(2)
    logging.debug("Attempting a recording of ITV to test-itv.ts on adaptor 1")
    xs.recordprogramme("ITV","/home/chris/test-itv.ts")
    time.sleep(2)
    printsfs(xs)
    logging.debug("recording for 30 seconds")
    time.sleep(30)
    logging.debug("Attempting to stop test-bbc-2.ts")
    s.stoprecording(filename="/home/chris/test-bbc-2.ts")
    logging.debug("Attempting to stop test-bbc-1.ts")
    s.stoprecording(filename="/home/chris/test-bbc-1.ts")
    logging.debug("Cleaning up filters")
    s.clean_sf()
    printsfs(s)
    logging.debug("Attempting to stop test-itv.ts")
    xs.stoprecording(filename="/home/chris/test-itv.ts")
    logging.debug("Attempting to stop test-4.ts")
    xs.stoprecording(filename="/home/chris/test-4.ts")
    logging.debug("Cleaning up filters")
    xs.clean_sf()
    printsfs(xs)
else:
    logging.error("dvbstreamer failed to start")
logging.debug("Test completed")
dvb.stopdvbstreamer()
