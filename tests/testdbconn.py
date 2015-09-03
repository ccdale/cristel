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

import sys
import os
import os.path as path
import logging
import logging.handlers

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

edb=EITDatabase(eitdb,log)
sdb=ScheduleDB(scheddb,log)

sql="select * from Channels order by logicalid asc"
chans=sdb.dosql(sql)
for chan in chans:
    sql="select count(*) as cn from Events where source='" + chan["source"] + "'"
    evs=edb.dosql(sql=sql,dictionary=1,one=1)
    print "%s programmes for channel %s" % ("{:4d}".format(evs["cn"]),chan["name"])
