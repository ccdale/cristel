<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * listings.php
 *
 * Started: Monday 27 July 2015, 17:14:53
 * Last Modified: Thursday 30 July 2015, 19:19:39
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

function makePage($page)/*{{{*/
{
    $op=<<<EOTXT
    <html>
    <head>
    <style>
a:link, a:visited {
    # display: block;
    color: #3A391A;
    # background-color: #98bf21;
    text-align: center;
    padding: 4px;
    text-decoration: none;
}

a:hover, a:active {
    # background-color: #7A991A;
    text-decoration: underline;
}
    </style>
    </head>
    <body>
    $page
    </body>
    </html>
EOTXT;
    return $op;
}/*}}}*/
/** // {{{ funtion GP($var,$trusted=false)
* returns the contents of the GET or POST variable
* favours POST over GET
*/
function GP($var,$trusted=false)
{
    $op=false;
    if(isset($_GET[$var])){
        $op=$_GET[$var];
    }
	if(isset($_POST[$var]))
	{
		$op=$_POST[$var];
	}
	if(is_string($op))
	{
        $op=trim(urldecode($op));
        if(!$trusted){
            $op=htmlspecialchars($op,ENT_QUOTES);
        }
		// $op=addslashes($op);
	}
	return $op;
} // }}}
function displayDay($ts=0)/*{{{*/
{
    if($ts){
        $op=date("D d",$ts);
    }else{
        $op=date("D d");
    }
    return $op;
}/*}}}*/
function displayDM($ts=0)/*{{{*/
{
    if($ts>0){
        $op=date("D d M",$ts);
    }else{
        $op=date("D d M");
    }
    return $op;
}/*}}}*/
function displayTime($ts=0)/*{{{*/
{
    if($ts){
        $op=date("H:i",$ts);
    }else{
        $op=date("H:i");
    }
    return $op;
}/*}}}*/
function ddt($ts)/*{{{*/
{
    return displayDay($ts) . " " . displayTime($ts);
}/*}}}*/
function displayDuration($prog)/*{{{*/
{
    $dt=ddt($prog["start"]);
    $duration=$prog["duration"]/60;
    return $dt . " " . $duration . " mins.";
}/*}}}*/
function channelDisplay($chan)/*{{{*/
{
    $op="<div id='" . $chan["source"] . "'>\n";
    $op.="<a href='listings.php?source=" . $chan["source"] . "'>" . $chan["name"] . "</a>\n";
    $op.="</div>\n";
    return $op;
}/*}}}*/
function titleattribute($prog,$withtime=false)/*{{{*/
{
    $title="";
    if($withtime){
        $title=displayDuration($prog) . "\n";
    }
    if(isset($prog["description"])){
        $title.=$prog["description"] . "\n";
    }
    if(isset($prog["content"])){
        $title.="programid: " . $prog["content"] . "\n";
    }
    if(isset($prog["series"])){
        $title.="seriesid: " . $prog["series"] . "\n";
    }
    return $title;
}/*}}}*/
function programmeDisplay($prog)/*{{{*/
{
    $progid=$seriesid=$description="";
    $op="<div class='progdisplay'";
    $title=titleattribute($prog);
    if(strlen($title)){
        $op.=" title=\"$title\"";
    }
    $op.=">\n";
    $op.="<span class='timedisplay'>";
    $op.=displayDuration($prog);
    $op.="</span>";
    $op.="<span class='titledisplay'>";
    $op.=$prog["title"];
    $op.="</span>";
    $op.="</div>\n";
    return $op;
}/*}}}*/
function makebannercell($name)/*{{{*/
{
    return "<td colspan='2' align='left'><a href='listings.php?channel=" . urlencode($name) . "'>$name</a></td>\n";
}/*}}}*/
function timerow($gridstart)/*{{{*/
{
    $gridwidth=3600*2;
    $gridend=$gridstart+$gridwidth;
    $pos=$gridstart;
    $previous=$gridstart-$gridwidth;
    $next=$gridstart+$gridwidth;
    $op="<tr class='timerow' style='background-color: #000;color: #fff;'>\n";
    $op.="<td class='arrowcellleft' align='left' style='background-color: #aaa'; color: #222;'><a href='listings.php?gridstart=$previous'><<</a></td>\n";
    $op.="<td class='arrowcellright' align='right' style='background-color: #aaa'; color: #222;'><a href='listings.php?gridstart=$next'>>></a></td>\n";
    while($pos<$gridend){
        $op.="<td class='timecell' colspan='6'>" . displayTime($pos) . "</td>\n";
        $pos+=1800;
    }
    $op.="</tr>\n";
    return $op;
}/*}}}*/
function grid($progs,$gridstart)/*{{{*/
{
    $next=$gridstart+(24*3600);
    $previous=$gridstart-(24*3600);
    $op="<p class='dayrow' align='center'><a class='dayarrow' href='listings.php?gridstart=$previous'><< </a><a href='listings.php'>" . displayDM($gridstart) . "</a><a href='listings.php?gridstart=$next'> >></a></p>\n";
    $op.="<table border=1>\n";
    $op.=timerow($gridstart);
    $rows=0;
    foreach($progs as $channelname=>$parr){
        $op.=gridrow($parr,makebannercell($channelname),$gridstart);
        $rows++;
        if($rows>7){
            $rows=0;
            $op.=timerow($gridstart);
        }
    }
    $op.="</table>\n";
    return $op;
}/*}}}*/
function gridrow($progs,$bannercell,$gridstart)/*{{{*/
{
    $gridwidth=3600*2;
    $gridend=$gridstart+$gridwidth;
    $slots=$gridwidth/60/5;// 5 minute slots over 2 hours
    $op="<tr>\n";
    $op.=$bannercell;
    foreach($progs as $prog){
        $start=$prog["start"]<$gridstart?$gridstart:$prog["start"];
        $end=$prog["end"]<=$gridend?$prog["end"]:$gridend;
        $nslots=($end-$start)/60/5;
        $op.=gridcell($prog,$start,$nslots);
    }
    $op.="</tr>\n";
    return $op;
}/*}}}*/
function gridcell($prog,$cellstart,$nslots)/*{{{*/
{
    $title=titleattribute($prog,true);
    $op="<td colspan='$nslots'";
    if(strlen($title)){
        $op.=" title=\"$title\"";
    }
    $op.=">";
    $op.=$prog["title"];
    $op.="</td>\n";
    return $op;
}/*}}}*/
$appdir="/home/chris/.epgdb";
$cristeldb=$appdir . "/cristel.db";
$epgdb=$appdir . "/database.db";
$chans=array();
$db=new SQLite3($cristeldb);
$res=$db->query("select * from channels where visible=1 order by logicalid ASC");
while($arr=$res->fetchArray()){
    $chans[$arr["logicalid"]]=array("name"=>$arr["name"],"source"=>$arr["source"],"logicalid"=>$arr["logicalid"],"priority"=>$arr["priority"],"muxid"=>$arr["muxid"]);
}
$db->close();
if(false===($start=GP("gridstart"))){
    $start=time();
}
$rem=$start % 1800;
$start=$start - $rem; // to the nearest 30 minutes
$end=$start+(24*3600);
if(false!==($channame=GP("channel"))){
    $channame=urldecode($channame);
    $progs=array();
    $op="<p>Channel not found: $channame</p>\n";
    $logicalid=0;
    foreach($chans as $chan){
        if($chan["name"]==$channame){
            $logicalid=$chan["logicalid"];
            break;
        }
    }
    if($logicalid){
        $db=new SQLite3($epgdb);
        $sql="select event,start,end,(end-start) as duration from Events where source='" . $chans[$logicalid]["source"] . "' and end > $start and start < $end order by start ASC";
        $res=$db->query($sql);
        while($arr=$res->fetchArray()){
            $prog=array(
                "event"=>$arr["event"],
                "start"=>$arr["start"],
                "end"=>$arr["end"],
                "source"=>$chans[$logicalid]["source"],
                "duration"=>$arr["duration"]
            );
            $sql="select * from Details where event=" . $prog["event"] . " and source='" . $prog["source"] . "'";
            $pres=$db->query($sql);
            while($arr=$pres->fetchArray()){
                $prog[$arr["name"]]=$arr["value"];
            }
            $progs[]=$prog;
        }
        $db->close();
        $cn=count($progs);
        if($cn){
            $op="<p><a href='listings.php'>Grid</a></p>\n";
            foreach($progs as $prog){
                $op.=programmeDisplay($prog);
            }
        }else{
            $op="<p>No programs found for " . $chans[$logicalid]["name"] . "</p>\n";
        }
        $op.="<p><a href='listings.php'>Grid</a></p>\n";
    }
}elseif(false!=($chanlist=GP("chanlist"))){
    $chandivs="";
    foreach($chans as $chan){
        $chandivs.=channelDisplay($chan);
    }
    $op=$chandivs;
}else{
    $end=$start+(3600*2);
    $db=new SQLite3($epgdb);
    $chanprogs=array();
    $logicalid=0;
    foreach($chans as $chan){
        $logicalid=$chan["logicalid"];
        $progs=array();
        if($logicalid){
            $db=new SQLite3($epgdb);
            $sql="select event,start,end,(end-start) as duration from Events where source='" . $chans[$logicalid]["source"] . "' and end > $start and start < $end order by start ASC";
            $res=$db->query($sql);
            while($arr=$res->fetchArray()){
                $prog=array(
                    "event"=>$arr["event"],
                    "start"=>$arr["start"],
                    "end"=>$arr["end"],
                    "source"=>$chans[$logicalid]["source"],
                    "duration"=>$arr["duration"]
                );
                $sql="select * from Details where event=" . $prog["event"] . " and source='" . $prog["source"] . "'";
                $pres=$db->query($sql);
                while($arr=$pres->fetchArray()){
                    $prog[$arr["name"]]=$arr["value"];
                }
                $progs[]=$prog;
            }
            $db->close();
        }
        $chanprogs[$chan["name"]]=$progs;
    }

    $op=grid($chanprogs,$start);
}
print makePage($op);
?>
