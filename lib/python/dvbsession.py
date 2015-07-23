"""
Module to extend (and fix) the dvbstreamer Session class
"""
# TODO: 
# check tuning status (festatus: 
# Tuner status: [ Signal, Lock, Carrier, VITERBI, Sync  ]
# Signal Strength: 61%
# SNR: 100%
# BER: 0
# Uncorrected Blocks: 789
# )
#
import time
import logging
from dvbstreamer.session import Session
from dvbstreamer.info import *

class DvbSession(Session):
    """Class represents an instance of a dvbstreamer server"""

    def __init__(self, host, adaptor, username='dvbstreamer', password='control'):
        Session.__init__(self,host,adaptor,username,password)

    def execute_command(self, command, authenticate=True):
        """
        Executes the specified command on the server.
        @param command: The command to execute.
        @param authenticate: Whether the connection needs to be authenticated
        before running the command.
        """
        logging.debug("attempting command: %s" % command)
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
                logging.debug("Added filter dvb%d" % sfnum)
                return sfnum
            else:
                logging.warning("Failed to add filter dvb%d" % sfnum)
        logging.error("filter number: <%s> out of bounds" % sfnum)
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
                logging.debug("Tuned to %s for filter %d" % (service,sfnum))
                return res
            else:
                logging.error("Failed to tune to %s for filter %d" % (service,sfnum))
                return False
        else:
            logging.error("Failed to select service %s for filter %d" % (service,sfnum))
            return False

    def set_mrl_sf(self,sfnum,mrl):
        """sets the output for the numbered service filter"""
        if sfnum == 0:
            cmd='setmrl %s' % mrl
        else:
            cmd='setsfmrl dvb%d %s' % (sfnum,mrl)
        emsg,res=self.execute_command(cmd)
        if emsg == "OK":
            logging.debug("set output for filter %d to %s" % (sfnum,mrl))
            return 1
        else:
            logging.error("failed to set output for filter %d to %s" % (sfnum,mrl))
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
                    logging.debug("removing filter dvb%d" % nsfs)
                    emsg,res=self.execute_command("rmsf dvb%d" % nsfs)
                    if emsg!="OK":
                        logging.warning("Failed to remove filter dvb%d" % nsfs)
                        # filter failed to remove, so stop
                        canclean=0
                else:
                    # the last service filter in the list is still in use, so stop
                    canclean=0
            else:
                logging.debug("Nothing left to clean")
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
                    logging.warning("It is not safe to tune to %s at this time" % channel)
                    logging.warning("Primary is tuned to %s and is in use" % sfs[0].service)
                    freesf=-1
                else:
                    logging.debug("Safe to tune to %s on free filter %d" % (channel,freesf))

        else:
            logging.error("Not safe as no free filter is available")
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
                    logging.debug("Recording %s into %s" % (channel,filename))
                    return 1
        logging.warning("Recording not starting for %s into %s" % (channel,filename))
        return 0

    def stoprecording(self,sfnum=None,filename=None):
        """stops a current recording

        either supply the service filter number or the recording filename
        both cannot be null
        @returns: int: true if service filter was updated, false otherwise
        """
        if sfnum==None:
            if  filename==None:
                logging.warning("Nothing to stop, no parameters supplied")
                return 0
        sfs=self.get_service_filters()
        if sfnum > -1:
            logging.debug("stopping by filter number: %d" % sfnum)
            res=self.set_mrl_sf(sfnum,"null://")
            return res
        count=0
        for sf in sfs:
            if ("file://%s" % filename)==sf.mrl:
                logging.debug("found filter for file: %s" % filename)
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
                logging.debug("Status: Sync")
                return True
        else:
            logging.error(status)
            return False
