<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * index.php
 *
 * Started: Sunday 16 August 2015, 02:21:57
 * Last Modified: Sunday 16 August 2015, 02:25:29
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

require_once "cristel.php";
?>
