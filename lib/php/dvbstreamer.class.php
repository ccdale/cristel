<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * dvbstreamer.class.php
 *
 * Started: Sunday  1 June 2014, 08:50:32
 * Last Modified: Thursday 24 July 2014, 20:44:47
 *
 * Copyright (c) 2014 Chris Allison chris.allison@hotmail.com
 *
 * This file is part of cristel.
 * 
 * cristel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cristel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with cristel.  If not, see <http://www.gnu.org/licenses/>.
 */

require_once "base.class.php";

class DvbStreamer extends Base
{
    private $adaptor=0;
    private $dvbsdir;
    private $cmd;
    private $pid=false;
    private $pidfile;
    private $user;
    private $pass;
    private $dontstop;

    public function __construct($logg=false,$adaptor=0,$user="tvc",$pass="tvc")/*{{{*/
    {
        parent::__construct($logg);
        $this->adaptor=$adaptor;
        $this->user=$user;
        $this->pass=$pass;
        $home=getenv("HOME");
        $this->dvbsdir=$this->unixPath($home) . ".dvbstreamer";
        $this->pidfile=$this->unixPath($this->dvbsdir) . "dvbstreamer-" . $this->adaptor . ".pid";
        $this->cmd="/usr/bin/dvbstreamer -a " . $this->adaptor . " -u " . $this->user . " -p " . $this->pass . " -d";
        if($this->isRunning()){
            $this->info("dvbstreamer for adaptor " . $this->adaptor . " is already running");
            $this->dontstop=true;
        }else{
            $this->info("dvbstreamer for adaptor " . $this->adaptor . " is not running.");
            $this->dontstop=false;
        }
        $this->startDvbstreamer();
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        $this->stopDvbstreamer();
        parent::__destruct();
    }/*}}}*/
    public function stopDvbstreamer()/*{{{*/
    {
        if(false==$this->dontstop){
            if(file_exists($this->pidfile)){
                $cpid=file_get_contents($this->pidfile);
                $procdir="/proc/$cpid";
                if(file_exists($procdir) && is_dir($procdir)){
                    $this->info("Stopping dvbstreamer for adaptor: " . $this->adaptor);
                    if(false===($junk=posix_kill($cpid,SIGTERM))){
                        $str=posix_strerror(posix_get_last_error());
                        $this->error("error killing dvbstreamer for adaptor: " . $this->adaptor);
                        $this->error("Posix says: $str");
                    }
                }
            }
        }else{
            $this->info("not attempting to stop dvbstreamer for adaptor: " . $this->adaptor . " as we didn't start it");
        }
    }/*}}}*/
    public function startDvbstreamer()/*{{{*/
    {
        if($this->dontstop){
            $this->debug("another process is already running for adaptor: " . $this->adaptor . " not starting another one.");
            return true;
        }else{
            $this->debug("starting process for adaptor: " . $this->adaptor);
            $lastline=exec($this->cmd,$op,$ecode);
            $this->debug("Checking to see if process started ok");
            // it would appear that it writes it's pid file straight
            // away.  we will need to wait a bit after the pid file
            // exists for it to fully start up, so give it 5 seconds
            sleep(5);
            // we may need to sleep a bit here to
            // allow dvbstreamer to write it't pid file
            // give it 10 seconds
            for($iter=0;$iter<10;$iter++){
                if($this->isRunning()){
                    $this->info("dvbstreamer started ok for adaptor: " . $this->adaptor);
                    return true;
                }
                $this->debug("waiting for dvbstreamer to start");
                sleep(1);
            }
            $this->error("failed to start dvbstreamer for adaptor: " . $this->adaptor);
            return false;
        }
    }/*}}}*/
    public function isRunning()/*{{{*/
    {
        $ret=false;
        if(file_exists($this->pidfile)){
            $this->debug("pidfile exists");
            $cpid=file_get_contents($this->pidfile);
            $pdir="/proc/$cpid";
            if(file_exists($pdir) && is_dir($pdir)){
                $this->debug("process from pidfile exists");
                $ret=true;
            }else{
                $this->debug("process from pidfile does not exist");
            }
        }
        return $ret;
    }/*}}}*/
}
?>
