<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * index.php
 *
 * Started: Sunday 16 August 2015, 02:21:57
 * Last Modified: Saturday 22 August 2015, 07:56:38
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

$logg=new Logging(false,"CPHP",0,LOG_INFO);
$b=new Base($logg);
$homedir="/home/chris";
$datadir=$b->unixpath($homedir) . ".epgdb";
$cristeldbfn=$b->unixpath($datadir) . "cristel.db";
$epgdbfn=$b->unixpath($datadir) . "database.db";

$cdb=new SSql($cristeldbfn,$logg);
$edb=new SSql($epgdbfn,$logg);

$eventkeys=array("start","end","source","duration","logicalid","content","description","series","title");

$gridstart=GP("gridstart");
$logg->debug("GP gridstart: $gridstart");
if(false!==($event=GP("event"))){
    $prog=array();
    foreach($eventkeys as $key){
        $prog[$key]=GP($key);
    }
    $pp=new Programme($cdb,$edb,$prog,$logg,$gridstart);
    $op=$pp->build();
}elseif(false!=($logicalid=GP("channel"))){
    $cp=new ChannelProgrammes($cdb,$edb,$logicalid,$logg,$gridstart);
    $op=$cp->build();
}else{
    $c=new Channel($logg,$cdb);
    $g=new Grid($cdb,$edb,$c->visiblechans(),$logg,$gridstart);
    $op=$g->build();
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
