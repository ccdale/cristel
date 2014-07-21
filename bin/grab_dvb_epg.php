#!/usr/bin/env php
<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * grab_dvb_epg.php
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Monday 21 July 2014, 02:33:36
 * Last Modified: Monday 21 July 2014, 02:56:51
 * Revision: $Id$
 * Version: 0.00
 */

function logInfo($logg,$arr)/*{{{*/
{
    $logg->info("Received " . $arr["numevents"] . " events");
    $logg->info("Updated " . $arr["updated"] . " schedule events");
    $logg->info($arr["dbinserted"] . " events inserted into the schedule");
    $logg->info($arr["ignored"] . " events have been ignored");
}/*}}}*/
function setUpIncludePath()/*{{{*/
{
    $pi=pathinfo(__FILE__);
    $binpath=$pi["dirname"];
    $tmp=explode("/",$binpath);
    $libpath="";
    foreach($tmp as $pp){
        if($pp=="bin"){
            break;
        }
        if($pp!=""){
            $libpath.="/" . $pp;
        }
    }
    $libpath.="/lib";
    // ensure that the apps classes get found first
    // by putting the lib directory at the front of the
    // include path.
    set_include_path($libpath . PATH_SEPARATOR . get_include_path());
}/*}}}*/

setUpIncludePath();
require_once "dvbstreamer.class.php";
require_once "simple-mysql.class.php";
require_once "logging.class.php";
require_once "epg.class.php";

$logg=new Logging(true,"DVBEPG",0,LOG_INFO,false,false,-1,LOG_INFO);
$mx=new Mysql($logg,"localhost","tvapp","tvapp","tv");
$dvb=new DvbStreamer($logg,0,"tvc","tvc");
$epg=new EPG($logg,"127.0.0.1","tvc","tvc",0,$dvb,$mx,false);

$epg->epgCapStart();
$nevents=0;
$killtime=3600; // 1 hour
$startcaptime=time();
while(true){
    if(false!==($arr=$epg->epgEvent())){
        if($arr["numevents"]>0 && 0==($arr["numevents"]%1000)){
            logInfo($logg,$arr);
            if($arr["stopnow"]){
                break;
            }
        }else{
            if($arr["stopnow"]){
                logInfo($logg,$arr);
                break;
            }
        }
    }
    if(time()>($startcaptime+$killtime)){
        logInfo($logg,$arr);
        $logg->info("times up");
        break;
    }
}
$logg->info("Shutting down");
$epg=null;
$dvb=null;
$mx=null;
$logg=null;
?>
