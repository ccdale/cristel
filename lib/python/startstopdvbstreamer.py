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
            print "Starting adadaptor: %d with username: %s, password: %s" % (a,username,password)
            os.system("dvbstreamer -d -a %d -u %s -p %s" % (a,username,password))
    else:
        print "dvbstreamer is already running"

def stopdvbstreamer():
    """
    runrecorder: starts up the cristel recorder.
    """
    pids=checkdvbstreamerrunning()
    l=len(pids)
    if l > 0:
        for pid in pids:
            print "killing Dvbstreamer at %d" % pid
            os.system("kill %d" % pid)
    else:
        print "Dvbstreamer is not running."

if __name__ == '__main__':
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
                    'usage: %prog [-sSk] [-u username] [-p password]')

        parser.add_option('-s', '--start', action="store_true", dest='start', help='start dvbstreamer')
        parser.add_option('-S', '--stop', action="store_true", dest='stop', help='stop dvbstreamer')
        parser.add_option('-k', '--kill', action="store_true", dest='stop', help='stop dvbstreamer')
        parser.add_option('-u', '--username', dest='username', metavar='USERNAME', help='connection username')
        parser.add_option('-p', '--password', dest='password', metavar='PASSWORD', help='connection password')
        (options, args) = parser.parse_args()
        if options.username:
            username=options.username
        if options.password:
            password=options.password
        if options.start:
            startdvbstreamer(numadaptors,username,password)
        if options.stop:
            stopdvbstreamer()
