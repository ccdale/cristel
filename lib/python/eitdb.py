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

"""eit db class"""

from cristeldb import CristelDB
from cristellog import CristelLog
import sqlite3

class EITDatabase(CristelDB):
    EVENTS_TABLE = 'Events'
    EVENTS_COLUMN_SOURCE= 'source'
    EVENTS_COLUMN_EVENT = 'event'
    EVENTS_COLUMN_START = 'start'
    EVENTS_COLUMN_END   = 'end'
    EVENTS_COLUMN_CA    = 'ca'
    EVENTS_COLUMNS = [EVENTS_COLUMN_SOURCE , EVENTS_COLUMN_EVENT, 
                        EVENTS_COLUMN_START  , EVENTS_COLUMN_END  , EVENTS_COLUMN_CA]
    
    RATINGS_TABLE = 'Ratings'
    RATINGS_COLUMN_SOURCE = 'source'
    RATINGS_COLUMN_EVENT  = 'event'
    RATINGS_COLUMN_SYSTEM = 'system'
    RATINGS_COLUMN_VALUE  = 'value'
    RATINGS_COLUMNS = [RATINGS_COLUMN_SOURCE, RATINGS_COLUMN_EVENT, RATINGS_COLUMN_SYSTEM, RATINGS_COLUMN_VALUE]
    
    DETAILS_TABLE = 'Details'
    DETAILS_COLUMN_SOURCE = 'source'
    DETAILS_COLUMN_EVENT  = 'event'
    DETAILS_COLUMN_NAME   = 'name'
    DETAILS_COLUMN_LANG   = 'lang'
    DETAILS_COLUMN_VALUE  = 'value'
    DETAILS_COLUMNS = [DETAILS_COLUMN_SOURCE, DETAILS_COLUMN_EVENT, DETAILS_COLUMN_NAME, 
                        DETAILS_COLUMN_LANG, DETAILS_COLUMN_VALUE]

    def __init__(self,eitdbfile,log=None):
        """sets up the EITDatabase class"""
        CristelDB.__init__(self,eitdbfile,log)
        self.debug("EITDatabase: creating tables (if not exist)")
        self.create_tables()

    def create_tables(self):
        # connection = sqlite3.Connection(self.dbpath)
        self.get_connection()
        cursor = self.connection.cursor()
        
        self.create_table(cursor, EITDatabase.EVENTS_TABLE,  EITDatabase.EVENTS_COLUMNS, 
                            [EITDatabase.EVENTS_COLUMN_SOURCE, EITDatabase.EVENTS_COLUMN_EVENT])
                              
        self.create_table(cursor, EITDatabase.RATINGS_TABLE, EITDatabase.RATINGS_COLUMNS, 
                            [EITDatabase.RATINGS_COLUMN_SOURCE, EITDatabase.RATINGS_COLUMN_EVENT,
                                EITDatabase.RATINGS_COLUMN_SYSTEM])
        
        self.create_table(cursor, EITDatabase.DETAILS_TABLE, EITDatabase.DETAILS_COLUMNS, 
                            [EITDatabase.DETAILS_COLUMN_SOURCE, EITDatabase.DETAILS_COLUMN_EVENT,
                                EITDatabase.DETAILS_COLUMN_NAME, EITDatabase. DETAILS_COLUMN_LANG])
        connection.close()
