"""
Module to extend (and fix) the dvbstreamer Session class
"""

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
                emsg, mrl = self.execute_command('getsfmrl %s' % sname)
                sfs.append(ServiceFilterInfo(sname,mrl,svc))
        return sfs

    def find_free_sf(self):
        """finds the first free service filter"""
        freesf=-1
        count=-1
        sfs=self.get_service_filters()
        for sf in sfs:
            count=count+1
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
                return sfnum
        return res 

    def set_sf(self,sfnum,service):
        """sets the service (channel) for the numbered service filter"""
        if (sfnum == 0):
            cmd='select "%s"' % service
        else:
            cmd='setsf dvb%d "%s"' % (sfnum,service)

        emsg,res=self.execute_command(cmd)
        if emsg == "OK":
            return res
        else:
            return False

    def set_mrl_sf(self,sfnum,mrl):
        """sets the output for the numbered service filter"""
        if sfnum == 0:
            cmd='setmrl %s' % mrl
        else:
            cmd='setsfmrl dvb%d %s' % (sfnum,mrl)
        emsg,res=self.execute_command(cmd)
        if emsg == "OK":
            return 1
        else:
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
                    emsg,res=self.execute_command("rmsf dvb%d" % nsfs)
                    if emsg!="OK":
                        # filter failed to remove, so stop
                        canclean=0
                else:
                    # the last service filter in the list is still in use, so stop
                    canclean=0
            else:
                # there are no service filters other than <Primary>
                canclean=0

    def safe_to_record(self,channel):
        """checks that it is safe to tune to channel on a free service filter"""
        freesf=self.find_free_sf()
        if freesf > -1:
            if freesf > 0:
                # <Primary> is in use so check what mux it is using
                sfs=self.get_service_filters()
                si=self.get_service_info(sfs[0].name)
                nsi=self.get_service_info(channel)
                if nsi.mux_uid != si.mux_uid:
                    freesf=-1

        return freesf

    def recordprogramme(self,channel,filename):
        """makes a recording into the named file of the named channel"""
        freesf=self.safe_to_record(channel)
        if freesf > -1:
            test=self.set_sf(freesf,channel)
            if test:
                test=self.set_mrl_sf(freesf,"file://%s" % filename)
                if test:
                    return 1
        return 0
