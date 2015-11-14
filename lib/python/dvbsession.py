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
"""
Module to extend (and fix) the dvbstreamer Session class
"""
import time
import logging
import logging.handlers
from dvbstreamer.session import Session
from dvbstreamer.info import *

# log=logging.getLogger(__name__)
# # log.setLevel(logging.DEBUG)
# handler=logging.handlers.SysLogHandler(address = '/dev/log', facility=logging.handlers.SysLogHandler.LOG_DAEMON)
# log.addHandler(handler)

log=None

class DvbSession(Session):
    """Class represents an instance of a dvbstreamer server"""

    def __init__(self, host, adaptor, username='dvbstreamer', password='control',logg=None):
        global log
        log=logg
        Session.__init__(self,host,adaptor,username,password)

    def execute_command(self, command, authenticate=True):
        """
        Executes the specified command on the server.
        @param command: The command to execute.
        @param authenticate: Whether the connection needs to be authenticated
        before running the command.
        """
        log.debug("attempting command: %s" % command)
        conn = self._get_connection(authenticate)
        try:
            result = conn.execute_command(command)
        finally:
            self.connections.append(conn)
        return result
    
    def get_service_filters(self):
        """
        retrieve a list of active service filters
        @returns: a list of ServiceFilterInfo objects
        """
        error_message, lines = self.execute_command('lssfs')
        sfs = []
        if len(lines):
            for sname in lines:
                emsg, svc = self.execute_command('getsf %s' % sname)
                if len(svc):
                    service=svc[0].split(" : ",1)
                    junk=service[1].split('"')
                    svc=junk[1]
                emsg, url = self.execute_command('getsfmrl %s' % sname)
                mrl=None
                if len(url):
                    mrl=url[0]
                sfs.append(ServiceFilterInfo(sname,mrl,svc))
        return sfs

    def find_free_sf(self):
        """finds the first free service filter"""
        freesf=-1
        count=-1
        # print "in find_free_sf"
        sfs=self.get_service_filters()
        for sf in sfs:
            count=count+1
            # print "checking sf %d" % count
            # print "testing sf.mrl: %s" % sf.mrl
            if sf.mrl is None:
                freesf=count
                break
            if sf.mrl == 'null://':
                freesf=count
                break
        if freesf==-1:
            count=count+1
            newsf=self.add_sf(count)
            if newsf == count:
                freesf=newsf
        return freesf

    def add_sf(self,sfnum):
        """
        adds a service filter name dvb<sfnum>
        sfnum cannot be zero
        @returns: the requested sf number or 0
        """
        res=0
        if (sfnum > -1) & (sfnum > 0):
            emsg, res = self.execute_command('addsf dvb%d null://' % sfnum)
            if emsg == "OK":
                log.debug("Added filter dvb%d" % sfnum)
                return sfnum
            else:
                log.warning("Failed to add filter dvb%d" % sfnum)
        log.error("filter number: <%s> out of bounds" % sfnum)
        return res 

    def set_sf(self,sfnum,service):
        """sets the service (channel) for the numbered service filter"""
        if (sfnum == 0):
            cmd='select "%s"' % service
        else:
            cmd='setsf dvb%d "%s"' % (sfnum,service)

        emsg,res=self.execute_command(cmd)
        if emsg == "OK":
            if self.waitfortune():
                log.debug("Tuned to %s for filter %d" % (service,sfnum))
                return res
            else:
                log.error("Failed to tune to %s for filter %d" % (service,sfnum))
                return False
        else:
            log.error("Failed to select service %s for filter %d" % (service,sfnum))
            return False

    def set_mrl_sf(self,sfnum,mrl):
        """sets the output for the numbered service filter"""
        if sfnum == 0:
            cmd='setmrl %s' % mrl
        else:
            cmd='setsfmrl dvb%d %s' % (sfnum,mrl)
        emsg,res=self.execute_command(cmd)
        if emsg == "OK":
            log.debug("set output for filter %d to %s" % (sfnum,mrl))
            return 1
        else:
            log.error("failed to set output for filter %d to %s" % (sfnum,mrl))
            return 0

    def clean_sf(self):
        """cleans up unused service filters"""
        canclean=1
        while canclean:
            sfs=self.get_service_filters()
            nsfs=len(sfs)
            # start at the last service filter
            if nsfs > 1:
                nsfs=nsfs-1
                if sfs[nsfs].mrl == "null://":
                    log.debug("removing filter dvb%d" % nsfs)
                    emsg,res=self.execute_command("rmsf dvb%d" % nsfs)
                    if emsg!="OK":
                        log.warning("Failed to remove filter dvb%d" % nsfs)
                        # filter failed to remove, so stop
                        canclean=0
                else:
                    # the last service filter in the list is still in use, so stop
                    canclean=0
            else:
                log.debug("Nothing left to clean")
                # there are no service filters other than <Primary>
                canclean=0

    def safe_to_record(self,channel):
        """checks that it is safe to tune to channel on a free service filter"""
        freesf=self.find_free_sf()
        # print "found %d as freesf" % freesf
        if freesf > -1:
            if freesf > 0:
                # <Primary> is in use so check what mux it is using
                sfs=self.get_service_filters()
                # print "checking %s" % sfs[0].service
                si=self.get_service_info(sfs[0].service)
                nsi=self.get_service_info(channel)
                if nsi.mux_uid != si.mux_uid:
                    log.warning("It is not safe to tune to %s at this time" % channel)
                    log.warning("Primary is tuned to %s and is in use" % sfs[0].service)
                    freesf=-1
                else:
                    log.debug("Safe to tune to %s on free filter %d" % (channel,freesf))

        else:
            log.error("Not safe as no free filter is available")
        return freesf

    def get_service_info(self, service):
        """
        Retrieve information about the specified service.
        @param service: The service to retrieve information for. This can either
        be a string descibing the name or id of the service or a Service object.
        @returns: A ServiceInfo object.
        """
        # print "getting service info: %s" % service
        error_message, lines = self.execute_command('serviceinfo "%s"' % service)
        return ServiceInfo(lines)
        
    def recordprogramme(self,channel,filename):
        """makes a recording into the named file of the named channel"""
        freesf=self.safe_to_record(channel)
        if freesf > -1:
            test=self.set_sf(freesf,channel)
            if test:
                test=self.set_mrl_sf(freesf,"file://%s" % filename)
                if test:
                    log.debug("Recording %s into %s" % (channel,filename))
                    return 1
        log.warning("Recording not starting for %s into %s" % (channel,filename))
        return 0

    def stoprecording(self,sfnum=None,filename=None):
        """stops a current recording

        either supply the service filter number or the recording filename
        both cannot be null
        @returns: int: true if service filter was updated, false otherwise
        """
        if sfnum==None:
            if  filename==None:
                log.warning("Nothing to stop, no parameters supplied")
                return 0
        sfs=self.get_service_filters()
        if sfnum > -1:
            log.debug("stopping by filter number: %d" % sfnum)
            res=self.set_mrl_sf(sfnum,"null://")
            return res
        count=0
        for sf in sfs:
            if ("file://%s" % filename)==sf.mrl:
                log.debug("found filter for file: %s" % filename)
                res=self.set_mrl_sf(count,"null://")
                return res
            count=count+1
        return 0

    def getstatus(self):
        """returns the front end status as a list"""
        emsg,res=self.execute_command("festatus")
        tmp=res[0].split("[")
        t2=tmp[1].split("]")
        s1=t2[0].split(",")
        status=[]
        for ss in s1:
            status.append(ss.strip())
        return status

    def waitfortune(self,timeout=10):
        count=0
        status=self.getstatus()
        while len(status)<5:
            time.sleep(1)
            count=count + 1
            if count > timeout:
                break;
            status=self.getstatus()
        if len(status)>4:
            if status[4] == "Sync":
                log.debug("Status: Sync")
                return True
        else:
            log.error(status)
            return False

    def logicalchannels(self):
        """returns the logical channel ids (freeview channel numbers) as a dict"""
        emsg,res=self.execute_command("lslcn")
        lcn={}
        for line in res:
            tmp=line.split(":")
            cn=tmp[0].strip()
            name=tmp[1].strip()
            lcn[cn]=name

        return lcn

    def logicalnames(self):
        """returns the logical channel ids (freeview channel numbers) as a dict keyed by name"""
        emsg,res=self.execute_command("lslcn")
        lcnames={}
        for line in res:
            tmp=line.split(":")
            cn=tmp[0].strip()
            name=tmp[1].strip()
            lcnames[name]=cn
        return lcnames

    def lsmuxes(self):
        """returns a list of muxes"""
        emsg,res=self.execute_command("lsmuxes")
        muxes=[]
        for line in res:
            muxes.append(line.strip())

        return muxes

    def servicesformux(self,mux):
        """returns a list of services for the supplied muxid"""
        emsg,res=self.execute_command("lsservices %s" % mux)
        svcs=[]
        for line in res:
            svcs.append(line.strip())

        return svcs

    def lsservices(self):
        """returns the complete list of services available"""
        emsg,res=self.execute_command("lsservices")
        svcs=[]
        for line in res:
            svcs.append(line.strip())
        return svcs
    
    def serviceinfo(self,service):
        """returns a hash of the service info for a service"""
        emsg,res=self.execute_command("serviceinfo '%s'" % service)
        si={}
        # for line in res.splitlines():
        for line in res:
            x=line.partition(':')
            n=x[0].strip(" \"\t\r\n")
            v=x[2].strip(" \"\t\r\n")
            si[n]=v
        return si

    def canscan(self):
        """waits until no scan is running

        scanning a non-existent mux will cause a generic command error
        the error message tells you whether a scan is currently running
        """
        while 1:
            try:
                emsg,res=self.execute_command("scan junk")
            except GenericCommandError as e:
                log.debug(e.msg)
                if e.msg.strip() == "Scan in progress!":
                    time.sleep(10)
                else:
                    log.debug("Scanning completed")
                    break

    def scannet(self,net):
        """scans a net for services"""
        self.canscan()
        log.info("scanning: %s" % net)
        emsg,res=self.execute_command("scan net '"+net+"'")
        self.canscan()
        log.info("Completed scan of %s" % net)
