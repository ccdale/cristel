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

    RSEARCH_TABLE = "rsearch"
    RSEARCH_COLUMN_TYPE = "type"
    RSEARCH_COLUMN_SEARCH = "search"
    RSEARCH_COLUMNS = [RSEARCH_COLUMN_TYPE,RSEARCH_COLUMN_SEARCH]

    SCHEDULE_TABLE = "schedule"
    SCHEDULE_COLUMN_ID = "id"
    SCHEDULE_COLUMN_SRC = "source"
    SCHEDULE_COLUMN_CNAME = "cname"
    SCHEDULE_COLUMN_EVENT = "event"
    SCHEDULE_COLUMN_MUXID = "muxid"
    SCHEDULE_COLUMM_START = "start"
    SCHEDULE_COLUMN_END = "end"
    SCHEDULE_COLUMN_TITLE = "title"
    SCHEDULE_COLUMN_DESC = "description"
    SCHEDULE_COLUMN_PROGID = "progid"
    SCHEDULE_COLUMN_SERIESID = "seriesid"
    SCHEDULE_COLUMN_ADAPTOR = "adaptor"
    SCHEDULE_COLUMNS = [SCHEDULE_COLUMN_SRC,SCHEDULE_COLUMN_CNAME,SCHEDULE_COLUMN_EVENT,SCHEDULE_COLUMN_MUXID,SCHEDULE_COLUMM_START,SCHEDULE_COLUMN_END,SCHEDULE_COLUMN_TITLE,SCHEDULE_COLUMN_DESC,SCHEDULE_COLUMN_PROGID,SCHEDULE_COLUMN_SERIESID,SCHEDULE_COLUMN_ADAPTOR]


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
        self.create_table(cursor, ScheduleDB.RSEARCH_TABLE, ScheduleDB.RSEARCH_COLUMNS,[ScheduleDB.RSEARCH_COLUMN_TYPE,ScheduleDB.RSEARCH_COLUMN_SEARCH])
        self.create_autoincrement_table(cursor,ScheduleDB.SCHEDULE_TABLE,ScheduleDB.SCHEDULE_COLUMNS,SCHEDULE_COLUMN_ID)
        self.connection.close()

    def updatelogical(self,data):
        self.get_connection()
        with self.connection:
            self.connection.executemany("update channels set logicalid=? where name=?",data)

    def updatemuxes(self,data):
        self.get_connection()
        with self.connection:
            self.connection.executemany("update channels set muxid=? where name=?",data)

    def scheduleevent(self,event):
        sql = "insert into schedule (source,cname,start,end,title,description,progid,seriesid) values ("
        sql += "'" + event["source"] "',"
        sql += "'" + event["cname"] "',"
        sql += "'" + event["start"] "',"
        sql += "'" + event["end"] "',"
        sql += "'" + event["title"] "',"
        sql += "'" + event["description"] "',"
        sql += "'" + event["progid"] "',"
        sql += "'" + event["seriesid"] "'"
        sql += ")"
        self.doinsertsql(sql)

    def getsearches(self):
        searches=()
        sql="select * from rsearch"
        rows=self.dosql(sql)
        for row in rows:
            search={"type":row["type"],"search":row["search"]}
            searches.append(search)
        return searches

    def getchannel(self,source):
        sql="select * from channels where source='" + source + "'"
        row=self.dosql(sql,one=1)
        return row
    
    def updateschedule(self,event):
        sql="insert or replace into schedule (id,"
        for field in ScheduleDB.SCHEDULE_COLUMNS:
            sql += field + ','
        sql =sql[:-1] + ") values ((select id from schedule where source='" + event["source"] + "' and event=" + event["event"] + "),"
        for field in ScheduleDB.SCHEDULE_COLUMNS:
            sql += event[field] + ','
        sql=sql[:-1] + ")"
        self.doinsertsql(sql)
