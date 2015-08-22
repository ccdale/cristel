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

"""Base Class to set up logging for all other classes"""

import logging
import logging.handlers

class CristelLog:
    """Use this as the base for all classes.

    setup like this (see main function below):
    log=logging.getLogger("cristel")
    log.setLevel(logging.DEBUG)
    handler=logging.handlers.SysLogHandler(address = '/dev/log',
              facility=logging.handlers.SysLogHandler.LOG_DAEMON)
    log.addHandler(handler)
    logger=CristelLog(log)
    """


    # should be an instance var, not a class var
    # logg=None

    def __init__(self,log=None):
        self.logg=log

    def debug(self,msg):
        """logging helper"""
        if self.logg is not None:
            self.logg.debug(msg)

    def info(self,msg):
        """logging helper"""
        if self.logg is not None:
            self.logg.info(msg)

    def notice(self,msg):
        """logging helper"""
        if self.logg is not None:
            self.logg.notice(msg)

    def info(self,msg):
        """logging helper"""
        if self.logg is not None:
            self.logg.info(msg)

    def warning(self,msg):
        """logging helper"""
        if self.logg is not None:
            self.logg.warning(msg)

    def warn(self,msg):
        """logging helper"""
        if self.logg is not None:
            self.logg.warning(msg)

    def error(self,msg):
        """logging helper"""
        if self.logg is not None:
            self.logg.error(msg)

    def critical(self,msg):
        """logging helper"""
        if self.logg is not None:
            self.logg.critical(msg)

def main():
    log=logging.getLogger("cristel")
    log.setLevel(logging.DEBUG)
    handler=logging.handlers.SysLogHandler(address = '/dev/log',
           facility=logging.handlers.SysLogHandler.LOG_DAEMON)
    log.addHandler(handler)
    logg=CristelLog(log)
    logg.info("MAIN in CristelLog activated")

if __name__ == "__main__":
        main()
