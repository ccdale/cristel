#!/usr/bin/env python2
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

"""Module for updating the cristel schedule database from the EIT database"""

import sys
import os
import os.path as path
import logging
import logging.handlers
import sqlite3
import xmltv

# setup the cristel lib directory
cristel_lib_dir = path.realpath(path.join(path.dirname(sys.argv[0]), '../lib/python'))
sys.path.append(cristel_lib_dir)

from scheduledb import ScheduleDB
from eitdb import EITDatabase

log=logging.getLogger("cristel")
log.setLevel(logging.DEBUG)
handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
log.addHandler(handler)

appdir=os.path.expanduser("~/.epgdb")
eitdb=os.path.join(appdir,"database.db")
scheddb=os.path.join(appdir,"cristel.db")
xmltvfile=os.path.join(appdir,"database.xmltv")

eit=EITDatabase(eitdb,log)
sch=ScheduleDB(scheddb,log)

sql=""

for ch in xmltv.read_channels(open(xmltvfile,'r')):
    name=ch["display-name"][0][0]
    xid=ch["id"]
    if len(sql):
        sql=sql + ",('" + name + "','" + xid + "',100,1,0,0,0)"
    else:
        sql="('" + name + "','" + xid + "',100,1,0,0,0)"

sql="insert into channels (name,source,priority,visible,favourite,logicalid,muxid) values " + sql
print sql
