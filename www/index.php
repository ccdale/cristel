<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * index.php
 *
 * Started: Sunday 16 August 2015, 02:21:57
 * Last Modified: Sunday 23 October 2016, 10:26:31
 * 
 * Copyright (c) 2015 Chris Allison chris.allison@hotmail.com
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

date_default_timezone_set("Europe/London");
$cristelpidfn="/var/tmp/cristel.pid";
$mypath=dirname(__FILE__);
/* this file should be in the /path/cristel/www directory, lets check */
$cristeldir=dirname($mypath);
$libpath=$cristeldir . DIRECTORY_SEPARATOR . "lib" . DIRECTORY_SEPARATOR . "php";
set_include_path($libpath . PATH_SEPARATOR . get_include_path());

// require_once "cristel.php";
require_once "logging.class.php";
require_once "simple-sqlite.class.php";
require_once "channel.class.php";
require_once "www.php";
require_once "grid.class.php";
require_once "channelprogrammes.class.php";
require_once "programmetable.class.php";
require_once "programme.class.php";

function doGrid()/*{{{*/
{
    global $cdb,$edb,$logg,$gridstart;
    $c=new Channel($logg,$cdb);
    $g=new Grid($cdb,$edb,$c->visiblechans(),$logg,$gridstart);
    $op=$g->build();
    return $op;
}/*}}}*/
function tellCristel()/*{{{*/
{
    global $cristelpidfn,$logg;
    exec("/home/chris/src/cristel/bin/updatecristel.py");
    if(file_exists($cristelpidfn)){
        $cristelpid=file_get_contents($cristelpidfn);
        exec("/home/chris/src/cristel/bin/cristelkill -s USR1 $cristelpid 2>&1",$killoutput,$killreturn);
        if($killreturn==0){
            $logg->info("successfully signalled cristel");
        }else{
            $killstr=implode("\n",$killoutput);
            $logg->warning("failed to signal cristel: output: $killstr");
        }
    }
}/*}}}*/
/* $logg=new Logging(false,"CPHP",0,LOG_DEBUG); */
$logg=new Logging(false,"CPHP",0,LOG_INFO);
$b=new Base($logg);
$homedir="/home/chris";
$datadir=$b->unixpath($homedir) . ".epgdb";
$cristeldbfn=$b->unixpath($datadir) . "cristel.db";
$epgdbfn=$b->unixpath($datadir) . "database.db";

$cdb=new SSql($cristeldbfn,$logg);
/* $iid=$cdb->insertCheck("Data",array('key'=>'timestamp','val'=>time())); */
$changed=$cdb->updateQuery("update Data set val=" . time() . " where key='timestamp'");
$edb=new SSql($epgdbfn,$logg);

$eventkeys=array("start","end","source","duration","logicalid","progid","description","seriesid","title");

$gridstart=GP("gridstart",true);
$logg->debug("GP gridstart: $gridstart");
if(false!==($event=GP("event",true))){
    $prog=array();
    foreach($eventkeys as $key){
        $prog[$key]=GP($key,true);
    }
    $pp=new Programme($cdb,$edb,$prog,$logg,$gridstart);
    $op=$pp->build();
}elseif(false!=($logicalid=GP("channel",true))){
    $cp=new ChannelProgrammes($cdb,$edb,$logicalid,$logg,$gridstart);
    $op=$cp->build();
}elseif(false!=($partsearch=GP("partsearch",true))){
    list($type,$value)=explode(":",$partsearch,2);
    $logg->info("list type: partsearch: ($partsearch), type: ($type), value: ($value)");
    $fieldarr=array("search"=>$value);
    switch($type){
    case "progid":
        $logg->info("record request for programid $value");
        $fieldarr["type"]="p";
        break;
    case "seriesid":
        $logg->info("record request for seriesid $value");
        $fieldarr["type"]="s";
        break;
    case "title":
        $logg->info("record request for title $value");
        $fieldarr["type"]="t";
        break;
    }
    $iid=$cdb->insertCheck("rsearch",$fieldarr);
    if($iid){
        $logg->debug("inserted record request ok type: $type, search: $value");
        tellCristel();
    }else{
        $logg->warning("failed to insert record request for type: $type, search: $value");
    }
    $op=doGrid();
}elseif(false!=($likesearch=GP("liketitlesearch",true)) && strlen($likesearch)){
    $logg->debug("request for like search: $likesearch");
    $fieldarr=array("search"=>$likesearch,"type"=>"l");
    $iid=$cdb->insertCheck("rsearch",$fieldarr);
    if($iid){
        $logg->debug("inserted record request ok type: $type, search: $value");
        tellCristel();
    }else{
        $logg->warning("failed to insert record request for type: $type, search: $value");
    }
    $op=doGrid();
}elseif(false!=($likesearch=GP("likedescsearch",true)) && strlen($likesearch)){
    $logg->debug("request for description like search: $likesearch");
    $fieldarr=array("search"=>$likesearch,"type"=>"d");
    $iid=$cdb->insertCheck("rsearch",$fieldarr);
    if($iid){
        $logg->debug("inserted record request ok type: $type, search: $value");
        tellCristel();
    }else{
        $logg->warning("failed to insert record request for type: $type, search: $value");
    }
    $op=doGrid();
}else{
    $op=doGrid();
}
?>
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->
    <title>Cristel TV</title>

    <!-- Bootstrap -->
    <link href="css/bootstrap.min.css" rel="stylesheet">
    <link href="css/cristel.css" rel="stylesheet">
  </head>
  <body>

    <!-- cristel tv content -->
    <?php print $op; ?>

    <!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
    <!-- Include all compiled plugins (below), or include individual files as needed -->
    <script src="js/bootstrap.min.js"></script>
  </body>
</html>
