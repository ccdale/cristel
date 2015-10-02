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

# import sqlite3
from cristeldb import CristelDB
# from cristellog import CristelLog

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
    RSEARCH_COLUMN_ID = "id"
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

    PREVIOUS_TABLE = "previous"
    PREVIOUS_COLUMN_ID = "id"
    PREVIOUS_COLUMN_SRC = "source"
    PREVIOUS_COLUMN_CNAME = "cname"
    PREVIOUS_COLUMN_EVENT = "event"
    PREVIOUS_COLUMN_MUXID = "muxid"
    PREVIOUS_COLUMM_START = "start"
    PREVIOUS_COLUMN_END = "end"
    PREVIOUS_COLUMN_TITLE = "title"
    PREVIOUS_COLUMN_DESC = "description"
    PREVIOUS_COLUMN_PROGID = "progid"
    PREVIOUS_COLUMN_SERIESID = "seriesid"
    PREVIOUS_COLUMN_ADAPTOR = "adaptor"
    PREVIOUS_COLUMNS = [PREVIOUS_COLUMN_SRC,PREVIOUS_COLUMN_CNAME,PREVIOUS_COLUMN_EVENT,PREVIOUS_COLUMN_MUXID,PREVIOUS_COLUMM_START,PREVIOUS_COLUMN_END,PREVIOUS_COLUMN_TITLE,PREVIOUS_COLUMN_DESC,PREVIOUS_COLUMN_PROGID,PREVIOUS_COLUMN_SERIESID,PREVIOUS_COLUMN_ADAPTOR]

    RECORDING_TABLE = "recording"
    RECORDING_COLUMN_ID = "id"
    RECORDING_COLUMN_SRC = "source"
    RECORDING_COLUMN_CNAME = "cname"
    RECORDING_COLUMN_EVENT = "event"
    RECORDING_COLUMN_MUXID = "muxid"
    RECORDING_COLUMM_START = "start"
    RECORDING_COLUMN_END = "end"
    RECORDING_COLUMN_TITLE = "title"
    RECORDING_COLUMN_DESC = "description"
    RECORDING_COLUMN_PROGID = "progid"
    RECORDING_COLUMN_SERIESID = "seriesid"
    RECORDING_COLUMN_ADAPTOR = "adaptor"
    RECORDING_COLUMN_FILE = "filepath"
    RECORDING_COLUMNS = [RECORDING_COLUMN_SRC,RECORDING_COLUMN_CNAME,RECORDING_COLUMN_EVENT,RECORDING_COLUMN_MUXID,RECORDING_COLUMM_START,RECORDING_COLUMN_END,RECORDING_COLUMN_TITLE,RECORDING_COLUMN_DESC,RECORDING_COLUMN_PROGID,RECORDING_COLUMN_SERIESID,RECORDING_COLUMN_ADAPTOR,RECORDING_COLUMN_FILE ]

    RECORDED_TABLE = "recorded"
    RECORDED_COLUMN_ID = "id"
    RECORDED_COLUMN_SRC = "source"
    RECORDED_COLUMN_CNAME = "cname"
    RECORDED_COLUMN_EVENT = "event"
    RECORDED_COLUMN_MUXID = "muxid"
    RECORDED_COLUMM_START = "start"
    RECORDED_COLUMN_END = "end"
    RECORDED_COLUMN_TITLE = "title"
    RECORDED_COLUMN_DESC = "description"
    RECORDED_COLUMN_PROGID = "progid"
    RECORDED_COLUMN_SERIESID = "seriesid"
    RECORDED_COLUMN_ADAPTOR = "adaptor"
    RECORDED_COLUMN_FILE = "filepath"
    RECORDED_COLUMNS = [RECORDED_COLUMN_SRC,RECORDED_COLUMN_CNAME,RECORDED_COLUMN_EVENT,RECORDED_COLUMN_MUXID,RECORDED_COLUMM_START,RECORDED_COLUMN_END,RECORDED_COLUMN_TITLE,RECORDED_COLUMN_DESC,RECORDED_COLUMN_PROGID,RECORDED_COLUMN_SERIESID,RECORDED_COLUMN_ADAPTOR,RECORDED_COLUMN_FILE]

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
        self.create_autoincrement_table(cursor,ScheduleDB.RSEARCH_TABLE,ScheduleDB.RSEARCH_COLUMNS,ScheduleDB.RSEARCH_COLUMN_ID)
        self.create_autoincrement_table(cursor,ScheduleDB.SCHEDULE_TABLE,ScheduleDB.SCHEDULE_COLUMNS,ScheduleDB.SCHEDULE_COLUMN_ID)
        self.create_autoincrement_table(cursor,ScheduleDB.PREVIOUS_TABLE,ScheduleDB.PREVIOUS_COLUMNS,ScheduleDB.PREVIOUS_COLUMN_ID)
        self.create_autoincrement_table(cursor,ScheduleDB.RECORDING_TABLE,ScheduleDB.RECORDING_COLUMNS,ScheduleDB.RECORDING_COLUMN_ID)
        self.create_autoincrement_table(cursor,ScheduleDB.RECORDED_TABLE,ScheduleDB.RECORDED_COLUMNS,ScheduleDB.RECORDED_COLUMN_ID)
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
        fields=('source','cname','event','muxid','start','end','title','description','progid','seriesid')
        sql="insert into schedule ("
        for field in fields:
            sql+=field + ","
        sql=sql[:-1] + ") values ("
        for field in fields:
            sql+="'" + event[field] +"',"
        sql=sql[:-1] + ")"
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

    def getname(self,name):
        sql="select * from channels where name='" + name + "'"
        row=self.dosql(sql,one=1)
        return row
    
    def newchan(self,source,name,mux):
        sql="insert into channels (source,name,priority,visible,favourite,logicalid,muxid) values ('" + source + "','" + name + "',0,0,0,0," + mux + ")"
        self.doinsertsql(sql)

    def updatechannel(self,source,name,mux):
        sql="update channels set source='" + source + "', muxid=" + mux + " where name='" + name + "'"
        self.doinsertsql(sql)

    def updateschedule(self,event):
        sql="insert or replace into schedule (id,"
        for field in ScheduleDB.SCHEDULE_COLUMNS:
            sql += field + ','
        sql =sql[:-1] + ") values ((select id from schedule where source='" + event["source"] + "' and event=" + event["event"] + "),"
        for field in ScheduleDB.SCHEDULE_COLUMNS:
            sql += event[field] + ','
        sql=sql[:-1] + ")"
        self.doinsertsql(sql)

    def updatesearch(self,xtype,search):
        sql="insert or replace into schedule (id,"
        for field in ScheduleDB.RSEARCH_COLUMNS:
            sql += field + ","
        sql=sql[:-1] + ") values ((select id from schedule where type='" + xtype + "' and search='" + search + "'),'" + xtype + "','" + search + "')"
        self.doinsertsql(sql)