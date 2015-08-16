<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * cristel.php
 *
 * Started: Sunday  2 August 2015, 16:21:46
 * Last Modified: Sunday 16 August 2015, 01:48:01
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

require_once "logging.class.php";
require_once "simple-sqlite.class.php";
require_once "channel.class.php";
require_once "www.php";
require_once "grid.class.php";

$logg=new Logging(false,"CPHP",0,LOG_DEBUG);
$b=new Base($logg);
$homedir="/home/chris";
$datadir=$b->unixpath($homedir) . ".epgdb";
$cristeldbfn=$b->unixpath($datadir) . "cristel.db";
$epgdbfn=$b->unixpath($datadir) . "database.db";

$cdb=new SSql($cristeldbfn,$logg);
$edb=new SSql($epgdbfn,$logg);
$c=new Channel($logg,$cdb);
$g=new Grid($cdb,$edb,$c->visiblechans(),$logg);
$op=$g->build();
print $op;
?>
