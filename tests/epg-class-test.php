#!/usr/bin/env php
<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * epg-class-test.php
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Wednesday 16 July 2014, 05:10:17
 * Last Modified: Monday 21 July 2014, 02:29:10
 * Revision: $Id$
 * Version: 0.00
 */

$pi=pathinfo(__FILE__);
$testpath=$pi["dirname"];
$tmp=explode("/",$testpath);
$libpath="";
foreach($tmp as $pp){
    if($pp=="tests"){
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
require_once "dvbstreamer.class.php";
require_once "simple-mysql.class.php";
require_once "logging.class.php";
require_once "epg.class.php";

$ecode=1;
$msg="Test Failed";

$logg=new Logging(true,"EPGTEST",0,LOG_DEBUG,false,false,-1,LOG_INFO);
$mx=new Mysql($logg,"localhost","tvapp","tvapp","tv");
$dvb=new DvbStreamer($logg,0,"tvc","tvc");
$epg=new EPG($logg,"127.0.0.1","tvc","tvc",0,$dvb,$mx,false);

$epg->epgCapStart();
$maxevents=32000;
$nevents=0;
$killtime=300;
$startcaptime=time();

while($nevents<$maxevents){
    if(false!==($arr=$epg->epgEvent())){
        $nevents=$arr["numevents"];
        $updated=$arr["updated"];
        $mismatchedtitle=$arr["mismatchedtitle"];
        $dbnotfound=$arr["dbnotfound"];
        $noseriesprogid=$arr["noseriesprogid"];
        $dbinserted=$arr["dbinserted"];
        $ignored=$arr["ignored"];
        $stopnow=$arr["stopnow"];
        $logg->debug("numevents $nevents, updated $updated, noseriespid $noseriesprogid, not found in db $dbnotfound, mis-matched title $mismatchedtitle, inserted into db $dbinserted, ignored $ignored");
    }else{
        $logg->debug("false returned from epg event");
    }
    if(time()>($startcaptime+$killtime)){
        $logg->info("times up");
        break;
    }
}
if($nevents>=$maxevents){
    $ecode=0;
    $msg="Tests passed";
}
$epg=null;
$dvb=null;
$mx=null;
$logg=null;
print "$msg\n";
exit($ecode);
?>
