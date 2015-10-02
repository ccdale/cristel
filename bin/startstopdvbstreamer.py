#!/usr/bin/env python2
# 
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
"""Starts or stops dvbstreamer daemons.

is designed to be hard linked to:
    stopdvbstreamer
    startdvbstreamer
"""

import os
import psutil
from optparse import OptionParser
import logging
import logging.handlers

log=logging.getLogger("cristel")

def checkdvbstreamerrunning():
    """return a list of dvbstreamer pids."""
    pids=[]
    for pid in psutil.pids():
        p=psutil.Process(pid)
        n=p.name()
        if n == 'dvbstreamer':
            pids.append(pid)

    return pids

def isdvbstreamerrunning():
    """returns true if dvbstreamer is running, false otherwise"""
    pids=checkdvbstreamerrunning()
    l=len(pids)
    if l == 0:
        return 0
    else:
        return 1

def startdvbstreamer(na,un,pw):
    """
    starts dvbstreamer if it isn't already running.
    
    param: na: number of adaptors to start
    param: un: connection username
    param: pw: connection password
    """
    if not isdvbstreamerrunning():
        for a in range(na):
            log.info("Starting adadaptor: %d with username: %s, password: %s" % (a,un,pw))
            os.system("dvbstreamer -d -a %d -u %s -p %s" % (a,un,pw))
    else:
        log.warning("dvbstreamer is already running")

def stopdvbstreamer():
    """
    stops any dvbstreamer instances
    """
    pids=checkdvbstreamerrunning()
    l=len(pids)
    if l > 0:
        for pid in pids:
            log.info("killing Dvbstreamer at %d" % pid)
            os.system("kill %d" % pid)
    else:
        log.warning("Dvbstreamer is not running.")

if __name__ == '__main__':
    log.setLevel(logging.DEBUG)
    handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
    log.addHandler(handler)
    prog=os.path.basename(__file__)
    numadaptors=2
    username='tvc'
    password='tvc'
    if (prog == "startdvbstreamer"):
        startdvbstreamer
    elif (prog == "stopdvbstreamer"):
        stopdvbstreamer
    else:
        parser = OptionParser(
                    'usage: %prog [-sk] [-u username] [-p password] [-n num-adaptors]')

        parser.add_option('-s', '--start', action="store_true", dest='start', help='start dvbstreamer')
        parser.add_option('-k', '--kill', action="store_true", dest='stop', help='stop dvbstreamer')
        parser.add_option('-u', '--username', dest='username', metavar='USERNAME', help='connection username (default: tvc)')
        parser.add_option('-p', '--password', dest='password', metavar='PASSWORD', help='connection password (default: tvc)')
        parser.add_option('-n', '--num-adaptors', dest='numadaptors', metavar='NUMADAPTORS', help='Number of adaptors to start/stop (default: 2)')
        (options, args) = parser.parse_args()
        if options.username:
            username=options.username
        if options.password:
            password=options.password
        if options.numadaptors:
            numadaptors=options.numadaptors
        if options.start:
            startdvbstreamer(numadaptors,username,password)
        if options.stop:
            stopdvbstreamer()
