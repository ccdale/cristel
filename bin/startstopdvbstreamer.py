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

def startindividualdvbs(host,an,un,pw):
    """
    starts dvbstreamer on adapater an

    param: host: the host to start it on (always 127.0.0.1, so ignored)
    param: an: adapter number to start
    param: un: connection username
    param: pw: connection password
    """
    log.info("Starting adadaptor: %d with username: %s, password: %s" % (an,un,pw))
    os.system("dvbstreamer -d -a %d -u %s -p %s" % (an,un,pw))

def startdvbstreamer(adapters):
    """
    starts dvbstreamer if it isn't already running.
    
    param: adapters: hash of adapter info [host,num,username,password]
    """
    if not isdvbstreamerrunning():
        for adapter in adapters:
            startindividualdvbs(*adapter)
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

def adapterline(line):
    host='127.0.0.0'
    adapter=0
    username='tvc'
    password='tvc'
    tmp=line.split(':')
    if len(tmp)==4:
        host=tmp[0]
        adapter=int(tmp[1])
        username=tmp[2]
        password=tmp[3]
    return (host,adapter,username,password)

def readadapters(appdir):
    adapters=[]
    adaptersfile = open(os.path.join(appdir, 'adapters'), 'r')
    for line in adaptersfile:
        line=line.strip()
        if line:
            adap=adapters.append(adapterline(line))
    return adapters

if __name__ == '__main__':
    log.setLevel(logging.DEBUG)
    handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
    log.addHandler(handler)
    prog=os.path.basename(__file__)
    appdir=os.path.expanduser("~/.epgdb")
    adapters=readadapters(appdir)
    if (prog == "startdvbstreamer"):
        startdvbstreamer(adapters)
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
            startdvbstreamer(adapters)
        if options.stop:
            stopdvbstreamer()
