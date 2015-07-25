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
#
"""Module for updating a mysql database from xmltv data"""
import sys
import os
import logging
import logging.handlers
import xmltv
import sqlite3
# from dvbstreamer.session import Session
# from dvbstreamer import events

# setup the cristel lib directory
# cristel_lib_dir = path.realpath(path.join(path.dirname(sys.argv[0]), '../lib/python'))
# sys.path.append(cristel_lib_dir)

# import startstopdvbstreamer as dvb
# from dvbsession import DvbSession

log=logging.getLogger("cristel")
log.setLevel(logging.DEBUG)
handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
log.addHandler(handler)

appdir=os.path.expanduser("~/.epgdb")
eitdb=os.path.join(appdir,"database.db")
scheddb=os.path.join(appdir,"cristel.db")
xmltvfile=os.path.join(appdir,"database.xmltv")

