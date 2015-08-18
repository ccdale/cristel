<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * programme.class.php
 *
 * Started: Sunday 16 August 2015, 12:26:32
 * Last Modified: Sunday 16 August 2015, 12:56:10
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

class Programme extends Base
{
    private $cx=false;
    private $ex=false;
    private $parr=false;
    private $pt=false;
    private $chan=false;
    private $cname=false;
    private $gridstart=false;

    public function __construct($cdb,$edb,$parr,$logg=false,$gridstart=false)/*{{{*/
    {
        parent::__construct($logg);
        $this->pt=new ProgrammeTable($logg);
        $this->cx=$cdb;
        $this->ex=$cdb;
        $this->parr=$parr;
        if(isset($this->parr["logicalid"])){
            $this->chan=new Channel($logg,$cdb,$this->parr["logicalid"]);
            $this->cname=$this->chan->getField("name");
        }
        $this->gridstart=$gridstart;
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
    public function build()/*{{{*/
    {
        $op="";
        if(isset($this->parr["title"])){
            $op="<div class='programme'>\n";
            $op.="<p class='programtitle'>" . $this->parr["title"] . "</p>\n";
            $op.="<p class='programtime'>" . $this->pt->ddt($this->parr["start"]) . " " . $this->displayDuration($this->parr["duration"]) . "</p>\n";
            $op.="<p class='programdesc'>" . $this->parr["description"] . "</p>\n";
            $op.="</div>\n";
            $op.="<p class='backblock'>";
            if(false!=$this->gridstart){
                $atts=array("gridstart"=>$this->gridstart);
            }else{
                $atts=false;
            }
            $op.=$this->pt->linkToSelf("back",$atts,"backlink","programme grid");
            $op.="</p>\n";
        }
        return $op;
    }/*}}}*/
    private function displayDuration($duration)/*{{{*/
    {
        $hours=intval($duration/3600);
        $rem=$duration%3600;
        $minutes=intval($rem/60);
        if($hours>0 && $minutes<10){
            $minutes="0" . $minutes;
        }
        return $hours . ":" . $minutes;
    }/*}}}*/
}
?>
