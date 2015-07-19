#!/usr/bin/env php
<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * cristel-class-test.php
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Wednesday 16 July 2014, 05:10:17
 * Last Modified: Wednesday 16 July 2014, 05:49:19
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
require_once "cristel.class.php";

$ecode=1;
$msg="Test Failed";
// start a new instance of the Cristel class.
$cx=new Cristel("tvapp",LOG_DEBUG);
if($cx->mx->amOK()){
    $ecode=0;
    $msg="Test Passed";
}
// stop the Cristel class
$cx=null;
print "$msg\n";
exit($ecode);
?>
