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

"""Schedule DB class"""

import sqlite3
from cristeldb import CristelDB
from cristellog import CristelLog

class ScheduleDB(CristelDB):
    CHANNELS_TABLE = "Channels"
    CHANNELS_COLUMN_SRC = "source"
    CHANNELS_COLUMN_NAME = "name"
    CHANNELS_COLUMN_PRIORITY = "priority"
    CHANNELS_COLOUMN_VISIBLE = "visible"
    CHANNELS_COLUMN_FAVOURITE = "favourite"
    CHANNELS_COLUMN_LOGICALID = "logicalid"
    CHANNELS_COLUMN_MUXID = "muxid"
    CHANNELS_COLUMNS = [CHANNELS_COLUMN_SRC, CHANNELS_COLUMN_NAME, CHANNELS_COLUMN_PRIORITY, CHANNELS_COLOUMN_VISIBLE,CHANNELS_COLUMN_FAVOURITE,CHANNELS_COLUMN_LOGICALID,CHANNELS_COLUMN_MUXID]

    def __init__(self,scheddbfile,log=None):
        """sets up the ScheduleDB class"""
        CristelDB.__init__(self,scheddbfile,log)
        self.debug("ScheduleDB: creating tables (if not exist)")
        self.create_tables()

    def create_tables(self):
        # connection = sqlite3.Connection(self.dbpath)
        self.get_connection()
        cursor = self.connection.cursor()
        
        self.create_table(cursor, ScheduleDB.CHANNELS_TABLE, ScheduleDB.CHANNELS_COLUMNS,[ScheduleDB.CHANNELS_COLUMN_SRC,ScheduleDB.CHANNELS_COLUMN_NAME])
        self.connection.close()

    def updatelogical(self,data):
        self.get_connection()
        with self.connection:
            self.connection.executemany("update channels set logicalid=? where name=?",data)

    def updatemuxes(self,data):
        self.get_connection()
        with self.connection:
            self.connection.executemany("update channels set muxid=? where name=?",data)
