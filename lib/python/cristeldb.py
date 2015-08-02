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

"""base sqlite3 database class"""
import sqlite3
from cristellog import CristelLog

class CristelDB(CristelLog):

    dbpath=None
    connection=None

    def __init__(self,dbfilename,log=None):
        """sets up the cristel db class"""
        CristelLog.__init__(self,log)
        self.dbpath=dbfilename
        self.debug("CristelDB started for file %s" % self.dbpath)
        # self.connection=sqlite3.Connection(self.dbpath)
    
    def create_autoincrement_table(self,cursor,table,fields,idfield):
        """see https://www.sqlite.org/autoinc.html item 2 
        for why this is not explicitly set to autoincrement.
        sqlite uses an unused integer if none is provided upon inser"""

        sql = 'CREATE TABLE IF NOT EXISTS ' + table + '('
        sql += idfield + " INTEGER PRIMARY KEY,"
        for field in fields:
            sql += field + ','
        sql =sql[:-1] + '));'

        self.debug("Create Table SQL: %s" % sql)
        
        cursor.execute(sql)

    def create_table(self, cursor, table, fields, primary_key):
        sql = 'CREATE TABLE IF NOT EXISTS ' + table + '('
        for field in fields:
            sql += field + ','
        sql += 'PRIMARY KEY('
        for field in primary_key:
            sql += field + ','
        sql =sql[:-1] + '));'

        self.debug("Create Table SQL: %s" % sql)
        
        cursor.execute(sql)

    def get_connection(self):
        self.connection=sqlite3.connect(self.dbpath)

    def dosql(self,sql,dictionary=1,one=0):
        self.get_connection()
        with self.connection:
            if dictionary==1:
                self.connection.row_factory = ScheduleDB.Row
            cursor = self.connection.cursor()
            cursor.execute(sql)
            if one>0:
                rows=cursor.fetchone()
            else:
                rows=cursor.fetchall()
        return rows

    def doinsertsql(self,sql):
        self.get_connection()
        with self.connection:
            cursor = self.connection.cursor()
            cursor.execute(sql)
