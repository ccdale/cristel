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

import sys
import os
import logging
import logging.handlers
import sqlite3
from scheduledb import ScheduleDB
from eitdb import EITDatabase

class Schedule(CristelLog):
    eit=None
    sch=None

    def __init__(self,log=None):
        appdir=os.path.expanduser("~/.epgdb")
        eitdb=os.path.join(appdir,"database.db")
        scheddb=os.path.join(appdir,"cristel.db")
        self.eit=EITDatabase(eitdb,log)
        self.sch=ScheduleDB(scheddb,log)

    def doschedule(self):
        searches=self.sch.getsearches()

# log=logging.getLogger("cristel")
# log.setLevel(logging.DEBUG)
# handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
# log.addHandler(handler)
# 
# xmltvfile=os.path.join(appdir,"database.xmltv")
# 
