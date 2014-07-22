#!/usr/bin/env php
<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * grab_rt_epg.php
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Monday 21 July 2014, 11:36:55
 * Last Modified: Monday 21 July 2014, 11:41:06
 * Revision: $Id$
 * Version: 0.00
 */

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
require_once "base.class.php";
require_once "logging.class.php";
require_once "simple-mysql.class.php";
require_once "radiotimes.class.php";
require_once "filldb.class.php";

$logg=new Logging(false,"RTEPG");
$mx=new Mysql($logg,"localhost","tvapp","tvapp","tv");
$rt=new RadioTimes($logg);
$fdb=new FillDb($logg,$mx,$rt);
$fdb->cleanDB();
$fdb=null;
$rt=null;
?>
