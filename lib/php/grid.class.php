<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * grid.class.php
 *
 * Started: Saturday 15 August 2015, 08:47:21
 * Last Modified: Wednesday 18 November 2015, 09:55:16
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

require_once "base.class.php";

class Grid extends Base
{
    private $cx=false;
    private $ex=false;
    private $start=false;
    private $width=false;
    private $slotsize=false;
    private $end=false;
    private $chanlist=false;
    private $ntimerows=10;
    private $pt=false;

    public function __construct($cdb,$edb,$chanlist,$logg=false,$start=false,$width=false,$slotsize=false,$ntimerows=20)/*{{{*/
    {
        parent::__construct($logg);
        $this->pt=new ProgrammeTable($logg);
        $this->cx=$cdb;
        $this->ex=$edb;
        $this->chanlist=$chanlist;
        $this->start=$this->defaultInt($start,time());
        $rem=$this->start % 1800; 
        $this->start=$this->start - $rem; // to the nearest 30 minutes
        $this->width=$this->defaultInt($width,7200);
        $this->slotsize=$this->defaultInt($slotsize,60*5); // 5 minutes
        $this->ntimerows=$this->defaultInt($ntimerows,20);
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
    public function setStart($start="") /*{{{*/
    {
        $this->start=$start;
    } /*}}}*/
    public function setWidth($width="") /*{{{*/
    {
        $this->width=$width;
    }  /*}}}*/
    public function setChanlist($chanlist="") /*{{{*/
    {
        $this->chanlist=$chanlist;
    }  /*}}}*/
    public function build()/*{{{*/
    {
        $op="<table border=1>\n";
        $op.=$this->dayrow();
        $gprogs=$this->gridProgrammes();
        if(false!==($cn=$this->ValidArray($gprogs))){
            $this->debug("$cn channels to display in grid");
            $op.=$this->timerow();
            $rows=0;
            foreach($gprogs as $logicalid=>$progs){
                $op.=$this->grow($progs,$logicalid);
                $rows++;
                if($rows>=$this->ntimerows){
                    $rows=0;
                    $op.=$this->timerow();
                }
            }
        }else{
            $this->warn("Cannot obtain a list of programmes for the grid");
        }
        $op.="</table>\n";
        return $op;
    }/*}}}*/
    private function defaultInt($xint,$default)/*{{{*/
    {
        $yint=intval($xint);
        if(false!==$xint && $yint>0){
            return $xint;
        }else{
            return $default;
        }
    }/*}}}*/
    private function amOK($logicalid=false)/*{{{*/
    {
        $ret=false;
        if(false!==$this->cx && false!==$this->ex){
            if($logicalid===false){
                $ret=true;
                $this->debug("database objects set ok");
            }else{
                if(false!==$this->chanlist){
                    if(false!==($cn=$this->ValidArray($this->chanlist))){
                        if(isset($this->chanlist[$logicalid]) && isset($this->chanlist[$logicalid]["source"])){
                            $ret=true;
                        }else{
                            $this->warn("Either the channel logical id is not set, or the channel source is not set. logicalid: $logicalid");
                        }
                    }else{
                        $this->warn("Channel list is not a valid array");
                    }
                }else{
                    $this->warn("Channel list not set");
                }
            }
        }else{
            $this->warn("one or both of the database objects is not set");
        }
        return $ret;
    }/*}}}*/
    private function gridProgrammes()/*{{{*/
    {
        $ret=false;
        if($this->amOK()){
            $gprogs=array();
            foreach($this->chanlist as $logicalid=>$channel){
                // $gprogs[$logicalid]=$this->channelProgrammes($logicalid);
                $gprogs[$logicalid]=$this->chanProgs($logicalid);
            }
            $ret=$gprogs;
        }
        return $ret;
    }/*}}}*/
    private function chanProgs($logicalid)/*{{{*/
    {
        $cprogs=false;
        if($this->amOK($logicalid)){
            $this->debug("logicalid: $logicalid, channel name: " . $this->chanlist[$logicalid]["name"]);
            $sql="select * from schedule where cname='" . $this->chanlist[$logicalid]["name"] . "' ";
            $sql.="and end > ";
            $sql.=$this->start;
            $sql.=" and start < ";
            $sql.=$this->start + $this->width;
            $sql.=" order by start ASC";
            if(false!==($rows=$this->cx->arrayQuery($sql))){
                foreach($rows as $row){
                    $arrtmp=$row;
                    $arrtmp["duration"]=$row["end"]-$row["start"];
                    $arrtmp["logicalid"]=$logicalid;
                    $cprogs[]=$arrtmp;
                }
            }else{
                $this->warn("no data returned for channel " . $this->chanlist[$logicalid]["name"]);
            }
        }else{
            $this->warn("not ok for logicalid: $logicalid, " . $this->chanlist[$logicalid]["name"]);
        }
        return $cprogs;
    }/*}}}*/
    private function channelProgrammes($logicalid)/*{{{*/
    {
        $cprogs=false;
        $this->debug("logicalid: $logicalid, channel name: " . $this->chanlist[$logicalid]["name"]);
        if($this->amOK($logicalid)){
            $sql=$this->chanEventSql($logicalid);
            if(false!==($events=$this->ex->arrayQuery($sql))){
                $cn=count($events);
                $this->debug("$cn events returned for logicalid: $logicalid");
                $cprogs=array();
                foreach($events as $event){
                    $cprogs[]=$this->progDetails($event,$logicalid);
                }
            }else{
                $this->warn("false returned for events sql for logicalid: $logicalid, " . $this->chanlist[$logicalid]["name"]);
            }
        }else{
            $this->warn("not ok for logicalid: $logicalid, " . $this->chanlist[$logicalid]["name"]);
        }
        return $cprogs;
    }/*}}}*/
    private function progDetails($event,$logicalid)/*{{{*/
    {
        $prog=array(
            "event"=>$event["event"],
            "start"=>$event["start"],
            "end"=>$event["end"],
            "source"=>$event["source"],
            "duration"=>$event["duration"],
            "logicalid"=>$logicalid
        );
        $sql="select * from Details where event=" . $prog["event"] . " and source='" . $prog["source"] . "'";
        $arr=$this->ex->arrayQuery($sql);
        foreach($arr as $val){
            $prog[$val["name"]]=$val["value"];
        }
        return $prog;
    }/*}}}*/
    private function chanEventSql($logicalid)/*{{{*/
    {
        $sql="select event,source,start,end,(end-start) as duration from Events where source='";
        $sql.=$this->chanlist[$logicalid]["source"];
        $sql.="' and end > ";
        $sql.=$this->start;
        $sql.=" and start < ";
        $sql.=$this->start + $this->width;
        $sql.=" order by start ASC";
        return $sql;
    }/*}}}*/
    private function gcell($content,$attrs=false)/*{{{*/
    {
        return $this->pt->tableData($attrs,$content);
    }/*}}}*/
    private function grow($progs,$logicalid)/*{{{*/
    {
        $content=$this->makeChannelCell($this->chanlist[$logicalid]);
        foreach($progs as $prog){
            $start=$prog["start"]<$this->start?$this->start:$prog["start"];
            $xend=$this->start+$this->width;
            $end=$prog["end"]<=$xend?$prog["end"]:$xend;
            $nslots=($end-$start)/$this->slotsize;
            $atts=array("title"=>$this->titleattribute($prog,true));
            $atts["colspan"]=$nslots;
            $atts["class"]="programcell" . $this->recordClass($prog["record"]);
            $content.=$this->gcell($this->makeProgContent($prog),$atts);
        }
        $op=$this->pt->tableRow(array("class"=>"gridrow"),$content);
        return $op;
    }/*}}}*/
    private function recordClass($record='n')/*{{{*/
    {
        switch($record){
        case 'l':
            $ret=" later";
            break;
        case 'p':
            $ret=" previous";
            break;
        case 'c':
            $ret=" conflict";
            break;
        case 'y':
            $ret=" record";
            break;
        case 'r':
            $ret=" recording";
            break;
        default:
            $ret="";
        }
        return $ret;
    }/*}}}*/
    private function makeProgContent($prog)/*{{{*/
    {
        $atts=$prog;
        $atts["gridstart"]=$this->start;
        return $this->pt->linkToSelf($prog["title"],$atts,"progcelllink");
    }/*}}}*/
    private function makeChannelCell($carr)/*{{{*/
    {
        $cname=$carr["name"];
        $logicalid=$carr["logicalid"];
        $atts=array("colspan"=>2,"class"=>"channelcell left");
        // $op="<td ";
        // $op.=$this->pt->makeAttsString($atts);
        // $op.=">";
        $content=$this->pt->linkToSelf($cname,array("channel"=>$logicalid));
        // $op.="</td>\n";
        $op=$this->pt->tableData($atts,$content);
        return $op;
    }/*}}}*/
    private function titleattribute($prog,$withtime=false)/*{{{*/
    {
        $title="";
        if($withtime){
            $title=$this->displayDuration($prog) . "\n";
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
    private function displayDuration($prog)/*{{{*/
    {
        $dt=$this->pt->ddt($prog["start"]);
        $duration=$prog["duration"]/60;
        return $dt . " " . $duration . " mins.";
    }/*}}}*/
    private function timerow()/*{{{*/
    {
        $gridend=$this->start+$this->width;
        $pos=$this->start;
        $previous=$this->start-$this->width;
        $next=$this->start+$this->width;
        $leftlink=$this->pt->linkToSelf("<",array("gridstart"=>$previous),"griddir");
        $rightlink=$this->pt->linkToSelf(">",array("gridstart"=>$next),"griddir");
        // $op="<tr class='timerow'>\n";
        $content=$this->gcell($leftlink,array("class"=>"arrowcell left"));
        $content.=$this->gcell($rightlink,array("class"=>"arrowcell right"));
        $colspan=($this->width/1800)+2; // width of the table + the 2 'channel columns'
        while($pos<$gridend){
            $content.=$this->gcell($this->pt->displayTime($pos),array("class"=>"timecell","colspan"=>$colspan));
            $pos+=1800;
        }
        // $op.="</tr>\n";
        $op=$this->pt->tableRow(array("class"=>"timerow"),$content);
        return $op;
    }/*}}}*/
    private function dayrow()/*{{{*/
    {
        $next=$this->start+(24*3600);
        $previous=$this->start-(24*3600);
        $now=time();
        $leftlink=$this->pt->linkToSelf(date("D",$previous),array("gridstart"=>$previous),"griddir");
        $rightlink=$this->pt->linkToSelf(date("D",$next),array("gridstart"=>$next),"griddir");
        $nowlink=$this->pt->linkToSelf(date("D",$this->start),array("gridstart"=>$now),"griddir");
        $fullwidth=intval($this->width/$this->slotsize)+2;
        $eachthree=intval($fullwidth/3);
        $rem=$fullwidth%3;
        // $op="<tr class='timerow'>\n";
        $content=$this->gcell($leftlink,array("class"=>"arrowcell left","colspan"=>$eachthree));
        $content.=$this->gcell($nowlink,array("class"=>"arrowcell centre","colspan"=>$eachthree+$rem));
        $content.=$this->gcell($rightlink,array("class"=>"arrowcell right","colspan"=>$eachthree));
        // $op.="</tr>\n";
        $op=$this->pt->tableRow(array("class"=>"timerow"),$content);
        return $op;
    }/*}}}*/
}
?>
