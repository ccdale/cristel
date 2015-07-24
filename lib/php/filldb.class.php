<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * filldb.class.php
 *
 * Started: Saturday 31 May 2014, 07:15:52
 * Last Modified: Thursday 24 July 2014, 20:44:49
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

/*
CREATE TABLE `channel` (
`rtid` int(11) NOT NULL,
`freeviewid` int(11) NOT NULL,
`channelname` varchar(50) NOT NULL,
`favourite` tinyint(1) DEFAULT '0',
`getdata` tinyint(1) DEFAULT '0',
`favgroup` tinyint(2) DEFAULT '0',
`sortorder` int(8) DEFAULT '0',
`hasplusone` int(11) DEFAULT '0',
PRIMARY KEY (`rtid`),
KEY `channelname` (`channelname`),
KEY `freeviewid` (`freeviewid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8

CREATE TABLE `schedule` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `channel` int(11) NOT NULL,
  `title` varchar(255) NOT NULL,
  `subtitle` varchar(255) DEFAULT NULL,
  `episode` varchar(255) DEFAULT NULL,
  `year` int(11) DEFAULT NULL,
  `director` int(11) DEFAULT NULL,
  `premiere` tinyint(1) DEFAULT NULL,
  `film` tinyint(1) DEFAULT NULL,
  `isrepeat` tinyint(1) DEFAULT NULL,
  `subtitles` tinyint(1) DEFAULT NULL,
  `widescreen` tinyint(1) DEFAULT NULL,
  `newseries` tinyint(1) DEFAULT NULL,
  `deafsigned` tinyint(1) DEFAULT NULL,
  `blackandWhite` tinyint(1) DEFAULT NULL,
  `star` tinyint(4) DEFAULT NULL,
  `certificate` tinyint(4) DEFAULT NULL,
  `description` text,
  `genre` int(11) DEFAULT NULL,
  `choice` tinyint(1) DEFAULT NULL,
  `starttime` int(11) NOT NULL,
  `endtime` int(11) NOT NULL,
  `duration` int(11) NOT NULL,
  `programid` varchar(128) DEFAULT NULL,
  `seriesid` varchar(128) DEFAULT NULL,
  `record` enum('c','l','n','p','y') default 'n',
  PRIMARY KEY (`id`),
  KEY `title` (`title`),
  KEY `description` (`description`(255)),
  KEY `channel` (`channel`),
  CONSTRAINT `schedule_ibfk_1` FOREIGN KEY (`channel`) REFERENCES `channel` (`rtid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8

CREATE TABLE `actor` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `actorname` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8

CREATE TABLE `actormap` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `actorid` int(11) NOT NULL,
  `scheduleid` int(11) NOT NULL,
  `chr` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `actorid` (`actorid`),
  KEY `scheduleid` (`scheduleid`),
  CONSTRAINT `actormap_ibfk_1` FOREIGN KEY (`actorid`) REFERENCES `actor` (`id`),
  CONSTRAINT `actormap_ibfk_2` FOREIGN KEY (`scheduleid`) REFERENCES `schedule` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
 */

require_once "base.class.php";

class FillDB extends Base
{
    private $mx=false;
    private $rt=false;
    private $fields;

    public function __construct($logg=false,$mx=false,$rt=false)/*{{{*/
    {
        parent::__construct($logg);
        $this->mx=$mx;
        $this->rt=$rt;
        $this->fields=array(
            array("title"=>"string"),
            array("subtitle"=>"string"),
            array("episode"=>"string"),
            array("year"=>"int"),
            array("director"=>"string"),
            array("cast"=>"array"),
            array("premiere"=>"bool"),
            array("film"=>"bool"),
            array("repeat"=>"bool"),
            array("subtitles"=>"bool"),
            array("widescreen"=>"bool"),
            array("newseries"=>"bool"),
            array("deafsigned"=>"bool"),
            array("blackandwhite"=>"bool"),
            array("filmstarrating"=>"int"),
            array("filmcertificate"=>"string"),
            array("genre"=>"string"),
            array("description"=>"string"),
            array("choice"=>"bool"),
            array("date"=>"string"),
            array("starttime"=>"int"),
            array("endtime"=>"int"),
            array("duration"=>"int")
        );
        $this->fillData();
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
    public function cleanDB()/*{{{*/
    {
        $yesterday=time()-86400;
        if(false!==($arr=$this->mx->arrayQuery("select id from schedule where endtime<$yesterday"))){
            foreach($arr as $sched){
                $this->deleteScheduleProgram($sched["id"]);
            }
        }
    }/*}}}*/
    public function fillData()/*{{{*/
    {
        if($this->mx && $this->rt){
            if(false!==($channels=$this->mx->arrayQuery("select * from channel where getdata=1"))){
                if(false!==($cn=$this->ValidArray($channels))){
                    $this->debug("$cn channels to get data for");
                    foreach($channels as $channel){
                        $this->info("Retrieving data for " . $channel["channelname"]);
                        if(false!==($progs=$this->rt->getChannelData($channel["rtid"]))){
                            $this->info("data translated");
                            if(false!==($cn=$this->ValidArray($progs))){
                                $this->info("$cn programs to insert into db for " . $channel["channelname"]);
                                $this->fillChannelData($channel["rtid"],$progs);
                                $this->info("Data in database updated");
                            }else{
                                $this->warning("no programs to insert into db for " . $channel["channelname"]);
                            }
                        }else{
                            $this->warning("no data received from radiotimes for " . $channel["channelname"]);
                        }
                    }
                }else{
                    $this->warning("No channel data retrieved from db");
                }
            }else{
                $this->error("Error getting channel data from db");
            }
        }else{
            $this->error("invalid class setup");
        }
    }/*}}}*/
    public function fillChannelData($cid=0,$progs=false)/*{{{*/
    {
        /*
         * schedule and actormap clearing are now done
         * bit by bit as new programs come in
         *
        $this->debug("Clearing actormap table for channel $cid");
        $rows=$this->mx->deleteQuery("delete from actormap where scheduleid in (select id from schedule where channel=$cid)");
        $this->debug("$rows rows deleted");
        $this->debug("Clearing schedule table for channelid $cid");
        $rows=$this->mx->deleteQuery("delete from schedule where channel=$cid");
        $this->debug("$rows rows deleted");
         */
        foreach($progs as $prog){
            $this->updateScheduleProgram($cid,$prog);
        }
    }/*}}}*/
    private function getGenreId($genre="")/*{{{*/
    {
        return $this->getStringId("genre",$genre);
    }/*}}}*/
    private function getActorId($actor="")/*{{{*/
    {
        return $this->getStringId("actor",$actor);
    }/*}}}*/
    private function getStringId($type="actor",$val="")/*{{{*/
    {
        $ret=false;
        $table=$type;
        $field=$type . "name";
        if(false!==$this->mx){
            if(false!==($cn=$this->ValidStr($val))){
                $tval=$this->mx->escape($val);
                $arr=$this->mx->arrayQuery("select id from $table where $field='$tval'");
                if(false!==($cn=$this->ValidArray($arr))){
                    $ret=$arr[0]["id"];
                }else{
                    $ret=$this->mx->insertQuery("insert into $table set $field='$tval'");
                }
            }
        }
        return $ret;
    }/*}}}*/
    private function updateScheduleProgram($channelid,$prog)/*{{{*/
    {
        if(false!==($arr=$this->whatOverlaps($channelid,$prog["starttime"],$prog["endtime"]))){
            $cn=count($arr);
            $title=$this->mx->escape($prog["title"]);
            if($cn>1){
                // this program replaces more than one 
                // delete all the others and insert this one
                $tmpstr="";
                foreach($arr as $dprog){
                    if(strlen($tmpstr)){
                        $tmpstr.=", " . $dprog["title"];
                    }else{
                        $tmpstr=$dprog["title"];
                    }
                    $this->deleteScheduleProgram($dprog["id"]);
                }
                $this->debug("New program replaces $cn others");
                $this->debug($title . " replaces $tmpstr");
                $this->insertProgram($channelid,$prog);
            }elseif($cn==1){
                // only one program, probably the same one
                // quick check if it is, and do nothing
                // otherwise replace
                if($title!=$arr[0]["title"]){
                    $this->debug("New program replaces 1 other");
                    $this->debug($title . " replaces " . $arr[0]["title"]);
                    $this->deleteScheduleProgram($arr[0]["id"]);
                    $this->insertProgram($channelid,$prog);
                }
            }else{
                // nothing in the db at this time, insert away
                $this->insertProgram($channelid,$prog);
            }
        }else{
            // nothing in  the db at this time
            $this->insertProgram($channelid,$prog);
        }
    }/*}}}*/
    private function insertProgram($cid,$prog)/*{{{*/
    {
        $genreid=$this->getGenreId($prog["genre"]);
        $did=$this->getActorId($prog["director"]);
        $sql="insert into schedule set ";
        $sql.="channel=$cid,";
        $sql.="title='" . $this->mx->escape($prog["title"]) . "',";
        if(false!==($cn=$this->ValidStr($prog["subtitle"])) && $cn>0){
            $sql.="subtitle='" . $this->mx->escape($prog["subtitle"]) . "',";
        }
        if(false!==($cn=$this->ValidStr($prog["episode"])) && $cn>0){
            $sql.="episode='" . $this->mx->escape($prog["episode"]) . "',";
        }
        if(false!==($cn=$this->ValidStr($prog["year"])) && $cn>0){
            $year=intval($prog["year"]);
            if($year>0){
                $sql.="year=$year,";
            }
        }
        $sql.="director=$did,";
        $tmp=$prog["film"]=="true"?1:0;
        $sql.="film=$tmp,";
        $tmp=$prog["repeat"]=="true"?1:0;
        $sql.="isrepeat=$tmp,";
        $tmp=$prog["subtitles"]=="true"?1:0;
        $sql.="subtitles=$tmp,";
        $tmp=$prog["widescreen"]=="true"?1:0;
        $sql.="widescreen=$tmp,";
        $tmp=$prog["newseries"]=="true"?1:0;
        $sql.="newseries=$tmp,";
        $tmp=$prog["deafsigned"]=="true"?1:0;
        $sql.="deafsigned=$tmp,";
        $tmp=$prog["blackandwhite"]=="true"?1:0;
        $sql.="blackandWhite=$tmp,";
        $tmp=strlen($prog["filmstarrating"])?$prog["filmstarrating"]:0;
        $sql.="star=$tmp,";
        $tmp=strlen($prog["filmcertificate"])?$prog["filmcertificate"]:0;
        $sql.="certificate=$tmp,";
        $sql.="description='" . $this->mx->escape($prog["description"]) . "',";
        $sql.="genre=$genreid,";
        $tmp=$prog["choice"]=="true"?1:0;
        $sql.="choice=$tmp,";
        $sql.="starttime=" . $prog["starttime"] . ",";
        $sql.="endtime=" . $prog["endtime"] . ",";
        $sql.="duration=" . $prog["duration"];
        if(false!==($iid=$this->mx->insertQuery($sql))){
            $this->debug("inserted: id: $iid title: " . $prog["title"]);
            $sql="";
            $actors="";
            if(false!==($cn=$this->ValidArray($prog["cast"]))){
                $sql="insert into actormap (actorid,scheduleid,chr) values ";
                foreach($prog["cast"] as $chr=>$actor){
                    $tchr=$this->mx->escape($chr);
                    if(false===($cn=$this->ValidArray($actor))){
                        $aid=$this->getActorId($actor);
                        if(strlen($actors)){
                            $actors.=",($aid,$iid,'$tchr')";
                        }else{
                            $actors="($aid,$iid,'$tchr')";
                        }
                    }else{
                        foreach($actor as $act){
                            $aid=$this->getActorId($act);
                            if(strlen($actors)){
                                $actors.=",($aid,$iid,'$tchr')";
                            }else{
                                $actors="($aid,$iid,'$tchr')";
                            }
                        }
                    }
                }
                if(strlen($actors)){
                    $sql.=$actors;
                    if(false!==($junk=$this->mx->insertQuery($sql))){
                        $this->debug("actors inserted ok");
                    }else{
                        $this->warning("failed to insert actors");
                        $this->debug($sql);
                    }
                }
            }
        }else{
            $this->warning("Failed to insert program " . $prog["title"]);
            $tmp=print_r($prog,true);
            $this->debug($tmp);
        }
    }/*}}}*/
    private function whatOverlaps($channelid=0,$start=0,$end=0)/*{{{*/
    {
        $ret=false;
        if($channelid>0 && $start>0 && $end>$start){
            $sql="select * from schedule where channel=$channelid and starttime between $start and $end and endtime between $start and $end";
            if(false!==($arr=$this->mx->arrayQuery($sql))){
                $ret=$arr;
            }
        }
        return $ret;
    }/*}}}*/
    private function deleteScheduleProgram($id=0)/*{{{*/
    {
        $ret=false;
        if($id>0){
            $sql="delete from actormap where scheduleid=$id";
            $rows=$this->mx->deleteQuery($sql);
            $this->debug("$rows rows deleted from actormap table");
            $sql="delete from schedule where id=$id";
            $rows=$this->mx->deleteQuery($sql);
            $this->debug("$rows rows deleted from schedule table");
            if($rows>0){
                $ret=true;
            }
        }
        return $ret;
    }/*}}}*/
}
?>
