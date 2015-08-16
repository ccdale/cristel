<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * grid.class.php
 *
 * Started: Saturday 15 August 2015, 08:47:21
 * Last Modified: Sunday 16 August 2015, 02:20:04
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

    public function __construct($cdb,$edb,$chanlist,$logg=false,$start=false,$width=false,$slotsize=false,$ntimerows=10)/*{{{*/
    {
        parent::__construct($logg);
        $this->cx=$cdb;
        $this->ex=$edb;
        $this->chanlist=$chanlist;
        $this->start=$this->defaultInt($start,time());
        $rem=$this->start % 1800; 
        $this->start=$this->start - $rem; // to the nearest 30 minutes
        $this->width=$this->defaultInt($width,7200);
        $this->slotsize=$this->defaultInt($slotsize,60*5); // 5 minutes
        $this->ntimerows=$this->defaultInt($ntimerows,10);
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
        $gprogs=$this->gridProgrammes();
        if(false!==($cn=$this->ValidArray($gprogs))){
            $this->debug("$cn channels to display in grid");
            $op.=$this->timerow();
            $rows=0;
            foreach($gprogs as $progs){
                $op.=$this->grow($progs);
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
    private function makeAttsString($atts=false)/*{{{*/
    {
        $op="";
        if(false!==($cn=$this->ValidArray($atts)) && $cn>0){
            foreach($atts as $key=>$val){
                $op.=$key . "=\"" . $val . "\" ";
            }
            $op=substr($op,0,-1);
        }
        return $op;
    }/*}}}*/
    private function defaultInt($xint,$default)/*{{{*/
    {
        if(false!==$xint && is_integer($xint) && $xint>0){
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
                $gprogs[$logicalid]=$this->channelProgrammes($logicalid);
            }
            $ret=$gprogs;
        }
        return $ret;
    }/*}}}*/
    private function channelProgrammes($logicalid)/*{{{*/
    {
        $cprogs=false;
        if($this->amOK($logicalid) && (false!==($events=$this->ex->arrayQuery($this->chanEventSql($logicalid))))){
            $cprogs=array();
            foreach($events as $event){
                $cprogs[]=$this->progDetails($event,$logicalid);
            }
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
    private function linkToSelf($name,$args=false,$class="",$title="")/*{{{*/
    {
        $op="<a ";
        $options="";
        if(false!==($cn=$this->ValidString($class)) && $cn>0){
            $op.="class='$class' ";
        }
        if(false!==($cn=$this->ValidString($title)) && $cn>0){
            $op.="title=\"$title\" ";
        }
        if(false!==($cn=$this->ValidArray($args)) && $cn>0){
            $options="?";
            foreach($args as $key=>$val){
                $options.=urlencode($key) . "=" . urlencode($val) . "&";
            }
            $options=substr($options,0,-1);
        }
        $op.="href='";
        $op.=$_SERVER["PHP_SELF"];
        $op.=$options;
        $op.="'>";
        $op.=$name;
        $op.="</a>\n";
        return $op;
    }/*}}}*/
    private function gcell($content,$attrs=false)/*{{{*/
    {
        $op="<td";
        if(false!==($cn=$this->ValidString($attrs)) && $cn>0){
            $op.=" " . $attrs;
        }elseif(false!==($cn=$this->ValidArray($attrs)) && $cn>0){
            $op.=" " . $this->makeAttsString($attrs);
        }
        $op.=">";
        $op.=$content;
        $op.="</td>\n";
        return $op;
    }/*}}}*/
    private function grow($progs)/*{{{*/
    {
        $op="<tr class='gridrow'>\n";
        $op.=$this->makeChannelCell($this->chanlist[$progs[0]["logicalid"]]["name"]);
        foreach($progs as $prog){
            $start=$prog["start"]<$this->start?$this->start:$prog["start"];
            $xend=$this->start+$this->width;
            $end=$prog["end"]<=$xend?$prog["end"]:$xend;
            $nslots=($end-$start)/$this->slotsize;
            $atts=array("title"=>$prog["description"]);
            $atts["colspan"]=$nslots;
            $atts["class"]="programcell";
            $op.=$this->gcell($this->makeProgContent($prog),$atts);
        }
        $op.="</tr>\n";
        return $op;
    }/*}}}*/
    private function makeProgContent($prog)/*{{{*/
    {
        return $this->linkToSelf($prog["title"],$prog,"progcelllink",$this->titleattribute($prog,true));
    }/*}}}*/
    private function makeChannelCell($cname)/*{{{*/
    {
        $atts=array("colspan"=>2,"align"=>"left","class"=>"channelcell");
        $op="<td ";
        $op.=$this->makeAttsString($atts);
        $op.=">";
        $op.=$cname;
        $op.="</td>\n";
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
        $dt=$this->ddt($prog["start"]);
        $duration=$prog["duration"]/60;
        return $dt . " " . $duration . " mins.";
    }/*}}}*/
    private function ddt($ts)/*{{{*/
    {
        return $this->displayDay($ts) . " " . $this->displayTime($ts);
    }/*}}}*/
    private function displayDay($ts=0)/*{{{*/
    {
        if($ts){
            $op=date("D d",$ts);
        }else{
            $op=date("D d");
        }
        return $op;
    }/*}}}*/
    private function displayDM($ts=0)/*{{{*/
    {
        if($ts>0){
            $op=date("D d M",$ts);
        }else{
            $op=date("D d M");
        }
        return $op;
    }/*}}}*/
    private function displayTime($ts=0)/*{{{*/
    {
        if($ts){
            $op=date("H:i",$ts);
        }else{
            $op=date("H:i");
        }
        return $op;
    }/*}}}*/
    private function timerow()/*{{{*/
    {
        $gridend=$this->start+$this->width;
        $pos=$this->start;
        $previous=$this->start-$this->width;
        $next=$this->start+$this->width;
        $leftlink=linkToSelf("<",array("gridstart"=>$previous),"griddir");
        $rightlink=linkToSelf(">",array("gridstart"=>$next),"griddir");
        $op="<tr class='timerow'>\n";
        $op.=$this->gcell($leftlink,array("class"=>"arrowcell left"));
        $op.=$this->gcell($rightlink,array("class"=>"arrowcell right"));
        while($pos<$gridend){
            $op.=gcell(displayTime($pos),array("class"=>"timecell"));
            $pos+=1800;
        }
        $op.="</tr>\n";
        return $op;
    }/*}}}*/
}
?>
