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

"""Module to operate the cristel schedule"""

# import sys
import os
import time
# import logging
# import logging.handlers
# import sqlite3
from cristellog import CristelLog
from scheduledb import ScheduleDB
from eitdb import EITDatabase

class Schedule(CristelLog):
    # should be instance vars, not class vars
    # eit=None
    # sch=None

    def __init__(self,log=None):
        # super(Schedule,self).__init__()
        self.logg=log
        appdir=os.path.expanduser("~/.epgdb")
        eitdb=os.path.join(appdir,"database.db")
        scheddb=os.path.join(appdir,"cristel.db")
        self.debug("starting eit db at %s" % eitdb)
        self.eit=EITDatabase(eitdb,log)
        self.debug("starting scheddb at %s" % scheddb)
        self.sch=ScheduleDB(scheddb,log)
        self.debug("scheduler class started")

    def completeevent(self,event):
        self.debug("completing event: %s" % str(event))
        chan=self.sch.getchannel(event["source"])
        self.debug("channel: %s" % str(chan))
        try:
            event["cname"]=chan["name"]
            event["muxid"]=chan["muxid"]
            event["visible"]=chan["visible"]
            event["favourite"]=chan["favourite"]
            event["priority"]=chan["priority"]
            event["logicalid"]=chan["logicalid"]
            self.debug("event completed %s" % str(event))
            return event
        except (IndexError,TypeError):
            self.warn("No channel for source %s" % event["source"])
            return False

    def makeschedule(self):
        events=[]
        searches=self.sch.getsearches()
        for search in searches:
            tevents=self.eit.getsearch(search)
            for event in tevents:
                testevent=self.completeevent(event)
                # self.debug(str(testevent))
                if testevent != False:
                    events.append(event)
        self.debug("Makeschedule: number of events to insert/update: %d" % len(events))
        for event in events:
            self.sch.updateschedule(event)
        return len(events)

    def getschedule(self):
        return self.sch.getschedule()
    
    def getnextschedule(self):
        return self.sch.getnextschedule()

    def getcurrentrecordings(self):
        return self.sch.getcurrentrecordings()

    def filldatabase(self):
        now=int(time.time())
        chans=self.sch.getvisiblechannels()
        cn=len(chans)
        self.debug("%d visible channels" % cn)
        for chan in chans:
            name=chan["name"]
            self.debug("getting programmes for channel %s" % name)
            source=chan["source"]
            progs=self.eit.getchannelevents(source,now)
            cn=len(progs)
            self.debug("%d programmes for channel %s" % (cn,name))
            for prog in progs:
                prog["cname"]=chan["name"]
                prog["muxid"]=chan["muxid"]
                prog["visible"]=chan["visible"]
                prog["favourite"]=chan["favourite"]
                prog["priority"]=chan["priority"]
                prog["logicalid"]=chan["logicalid"]
                self.sch.updateschedule(prog)
        n24=now-(24*60*60)
        self.sch.reapschedule(n24)
