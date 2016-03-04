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

import time
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
    SCHEDULE_COLUMN_RECORD = "record"
    SCHEDULE_COLUMNS = [SCHEDULE_COLUMN_SRC,SCHEDULE_COLUMN_CNAME,SCHEDULE_COLUMN_EVENT,SCHEDULE_COLUMN_MUXID,SCHEDULE_COLUMM_START,SCHEDULE_COLUMN_END,SCHEDULE_COLUMN_TITLE,SCHEDULE_COLUMN_DESC,SCHEDULE_COLUMN_PROGID,SCHEDULE_COLUMN_SERIESID,SCHEDULE_COLUMN_ADAPTOR,SCHEDULE_COLUMN_RECORD]
    SCHEDULE_INSERT_COLUMNS = [SCHEDULE_COLUMN_SRC,SCHEDULE_COLUMN_CNAME,SCHEDULE_COLUMN_EVENT,SCHEDULE_COLUMN_MUXID,SCHEDULE_COLUMM_START,SCHEDULE_COLUMN_END,SCHEDULE_COLUMN_TITLE,SCHEDULE_COLUMN_DESC,SCHEDULE_COLUMN_PROGID,SCHEDULE_COLUMN_SERIESID]
    SCHEDULE_INSERT_QUOTE = {"source":1,"cname":1,"event":0,"muxid":0,"start":0,"end":0,"title":1,"description":1,"progid":1,"seriesid":1}

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
        self.later=0
        self.record=0
        self.conflict=0
        self.previous=0

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

    def dosearches(self):
        self.info("updating schedule")
        self.later=0
        self.record=0
        self.conflict=0
        self.previous=0
        sql="select * from rsearch"
        rows=self.dosql(sql)
        cn=len(rows)
        self.debug("{} searches to find".format(cn))
        for row in rows:
            search={"type":row["type"],"search":row["search"]}
            self.debug("search type:{}, search for: {}".format(search["type"],search["search"]))
            self.schedulesearch(search)
        self.info("updated schedule: record:{} later:{} previous:{} conflict:{}".format(self.record,self.later,self.previous,self.conflict))

    def schedulesearch(self,search):
        res=None
        if search["type"]=='t':
            res=self.findprograms(field='title',search=search["search"])
            cn=len(res)
            self.debug("found {} programmes for {}".format(cn,search["search"]))
        elif search["type"]=='l':
            res=self.findprograms(field='title',search=search["search"],like=1)
            cn=len(res)
            self.debug("found {} programmes for {}".format(cn,search["search"]))
        elif search["type"]=='p':
            res=self.findprograms(field='progid',search=search["search"])
            cn=len(res)
            self.debug("found {} programmes for {}".format(cn,search["search"]))
        elif search["type"]=='s':
            res=self.findprograms(field='seriesid',search=search["search"])
            cn=len(res)
            self.debug("found {} programmes for {}".format(cn,search["search"]))
        elif search["type"]=='d':
            res=self.findprograms(field='description',search=search["search"],like=1)
            cn=len(res)
            self.debug("found {} programmes for {}".format(cn,search["search"]))
        if res is not None:
            for row in res:
                self.scheduleprogram(row)

    def findprograms(self,field,search,like=0):
        sql=u"select * from schedule where "
        sql+=field
        if like==0:
            sql+="='{}'".format(search)
        else:
            sql+=" like '%{}%'".format(search)
        sql+=" and record='n'"
        return self.dosql(sql)

    def scheduleprogram(self,prog):
        overlaps=self.hasoverlaps(prog)
        self.dorecord(prog,overlaps)

    def hasoverlaps(self,prog):
        sql=u"select * from schedule where start<{} and end>{} and id != {} and record='y'".format(prog["end"],prog["start"],prog["id"])
        return self.dosql(sql)

    def dorecord(self,prog,overlaps):
        diffmux=0
        mymux=prog["muxid"]
        if self.checkprevious(prog):
            record='p'
            self.previous=self.previous+1
            self.debug("marking programme {} ({}) as previously recorded".format(prog["title"],prog["progid"]))
        else:
            for overlap in overlaps:
                if (mymux is not overlap["muxid"]):
                    diffmux=diffmux+1
            if diffmux<=1:
                record='y'
                self.debug("marking programme {} ({}) to be recorded".format(prog["title"],prog["progid"]))
                self.record=self.record+1
            else:
                if self.checklater(prog):
                    record='l'
                    self.debug("marking programme {} ({}) to be recorded at a later time".format(prog["title"],prog["progid"]))
                    self.later=self.later+1
                else:
                    record='c'
                    self.debug("marking programme {} ({}) to be in conflict".format(prog["title"],prog["progid"]))
                    self.conflict=self.conflict+1
        sql=u"update schedule set record='{}' where id={}".format(record,prog["id"])
        self.doinsertsql(sql)

    def checkprevious(self,prog):
        sql=u"select * from previous where progid='{}'".format(prog["progid"])
        rows=self.dosql(sql)
        if (len(rows) > 0 ):
            return True
        else:
            return False
    
    def checklater(self,prog):
        sql=u"select * from schedule where progid='{}' and id != {}".format(prog["progid"],prog["id"])
        rows=self.dosql(sql)
        if (len(rows) > 0 ):
            return True
        else:
            return False

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

    def newlchan(self,source,name,mux,lcn):
        sql="insert into channels (source,name,priority,visible,favourite,logicalid,muxid) values ('" + source + "','" + name + "',0,0,0," + lcn + "," + mux + ")"
        log.debug(sql)
        self.doinsertsql(sql)

    def updatechannel(self,source,name,mux):
        sql="update channels set source='" + source + "', muxid=" + mux + " where name='" + name + "'"
        self.doinsertsql(sql)

    def updatelchannel(self,source,name,mux,lcn):
        sql="update channels set source='" + source + "', muxid=" + mux + ", logicalid=" + lcn + " where name='" + name + "'"
        # sql="update channels set source='%s',muxid=%d,logicalid=%d where name='%s'" % (source,int(mux),int(lcn),name)
        self.doinsertsql(sql)

    def updateschedule(self,event):
        sql=u"select * from schedule where source='{}' and event={} and start={} and end={}".format(event["source"],event["event"],event["start"],event["end"])
        rows=self.dosql(sql)
        cn=len(rows)
        if cn==0:
            sql=u"insert into schedule ("
            for field in ScheduleDB.SCHEDULE_INSERT_COLUMNS:
                sql += field + ','
            sql+=ScheduleDB.SCHEDULE_COLUMN_RECORD
            sql += ") values ("
            uerrors=0
            for field in ScheduleDB.SCHEDULE_INSERT_COLUMNS:
                try:
                    tmp = self.formatfield(event[field],ScheduleDB.SCHEDULE_INSERT_QUOTE,field)
                except KeyError:
                    self.error("KeyError: %s" % field)
                    tmp="No Data"
                try:
                    sql += tmp + ','
                except (UnicodeDecodeError,UnicodeEncodeError):
                    self.error("unicode decode error? field: %s, tmp: %s, sql: %s" % (field,tmp,sql))
                    print "unicode decode error: field: %s, tmp: %s, sql: %s" % (field,tmp,sql)
                    uerrors=uerrors+1
            if uerrors == 0:
                sql+= '"n")'
                try:
                    self.doinsertsql(sql)
                except:
                    self.error("error inserting the following sql: %s" % sql)
                    print "error inserting the following sql: %s" % sql
            else:
                self.warn("There were errors in inserting this event: %s" % str(event))
            return True
        else:
            return False

    def formatfield(self,data,xdict,field):
        if xdict[field] == 1:
            x=u'"' + data.replace('"',"'") + '"'
        else:
            x=str(data)
        return x

    def escapestring(self,data):
       return data.replace("'","\\'")

    def updatesearch(self,xtype,search):
        sql="insert or replace into schedule (id,"
        for field in ScheduleDB.RSEARCH_COLUMNS:
            sql += field + ","
        sql=sql[:-1] + ") values ((select id from schedule where type='" + xtype + "' and search='" + search + "'),'" + xtype + "','" + search + "')"
        self.doinsertsql(sql)

    def getschedule(self):
        now=int(time.time())
        # sql="select * from schedule where start>%d and start<%d order by start" % (now,now+3600)
        sql="select * from schedule where start>%d and record='y' order by start asc" % now
        return self.dosql(sql)

    def getnextschedule(self):
        now=int(time.time())
        sql="select * from schedule where start>%d and record='y' order by start asc limit 1" % now
        return self.dosql(sql,one=1)

    def getcurrentrecordings(self):
        sql="select * from recording order by end asc"
        return self.dosql(sql)

    def getvisiblechannels(self):
        sql="select * from Channels where visible=1"
        rows=self.dosql(sql)
        return rows

    def reapschedule(self,before):
        sql="delete from schedule where end<" + str(before)
        self.dosql(sql)
