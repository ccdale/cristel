<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * channelprogrammes.class.php
 *
 * Started: Sunday 16 August 2015, 04:24:33
 * Last Modified: Sunday 16 August 2015, 13:49:38
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

class ChannelProgrammes extends Base
{
    private $logicalid=false;
    private $cx=false;
    private $ex=false;
    private $ch=false;
    private $progs=false;
    private $gridstart=false;
    private $pt=false;

    public function __construct($cdb,$edb,$logicalid,$logg=false,$gridstart=false)/*{{{*/
    {
        parent::__construct($logg);
        $this->pt=new ProgrammeTable($logg);
        $this->cx=$cdb;
        $this->ex=$edb;
        $this->logicalid=$logicalid;
        $this->ch=new Channel($logg,$this->cx,$this->logicalid);
        $this->gridstart=$gridstart;
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
    public function getProgs()/*{{{*/
    {
        if(false!==$this->gridstart){
            $start=$this->gridstart;
        }else{
            $start=time();
        }
        $end=$start+(24*3600);
        $esql="select * from events where source='";
        $esql.=$this->ch->getField("source");
        $esql.="' and end>$start and start<$end order by start asc";
        $events=$this->ex->arrayQuery($esql);
        $progs=array();
        foreach($events as $event){
            $sql="select * from Details where source='";
            $sql.=$this->ch->getField("source");
            $sql.="' and event=";
            $sql.=$event["event"];
            $prog=$this->ex->arrayQuery($sql);
            foreach($prog as $item){
                $event[$item["name"]]=$item["value"];
            }
            $progs[]=$event;
        }
        return $progs;
    }/*}}}*/
    public function build()/*{{{*/
    {
        $content="";
        $progs=$this->getProgs();
        foreach($progs as $prog){
            $content.=$this->progrow($prog);
        }
        if($this->gridstart){
            $args=array("gridstart"=>$this->gridstart);
        }else{
            $args=false;
        }
        $lnk=$this->pt->linkToSelf("back",$args,"backlink","Programme grid");
        $content.=$this->pt->tableRow(array("class"=>"backrow"),$this->pt->tableData(array("colspan"=>2,"class"=>"backcell"),$lnk));
        $atts=array("border"=>1);
        return $this->pt->Table($atts,$content);
    }/*}}}*/
    private function backlink()/*{{{*/
    {
        if(false!=$this->gridstart){
            $atts=array("gridstart"=>$this->gridstart);
        }else{
            $atts=false;
        }
        return $this->pt->linkToSelf("back",$atts,"backlink","programme grid");
    }/*}}}*/
    private function progrow($prog)/*{{{*/
    {
        $atts=array("class"=>"progtimecell");
        $content=$this->pt->tableData($atts,$this->pt->displayTime($prog["start"]));
        $atts["class"]="progtitlecell";
        $atts["title"]=$prog["description"];
        $content.=$this->pt->tableData($atts,$prog["title"]);
        $atts["class"]='progrow';
        unset($atts["title"]);
        return $this->pt->tableRow($atts,$content);
    }/*}}}*/
}
?>
