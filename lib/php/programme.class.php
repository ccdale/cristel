<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * programme.class.php
 *
 * Started: Sunday 16 August 2015, 12:26:32
 * Last Modified: Saturday 22 August 2015, 17:33:35
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
/*
 * programme array fields:
 * 
    [start] => 1440244800
    [end] => 1440248400
    [source] => 233a.5040.5ca0
    [duration] => 3600
    [logicalid] => 17
    [content] => bds.tv/ABSJCJ
    [description] => Property show jam-packed with top tips on how to bag a bargain. Here, a property in Derby, a west London flat and a property in Plymouth which is an attack on the senses! [S]
    [series] => bds.tv/N25BG0
    [title] => Homes under the Hammer
 *
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
        $tmp=print_r($this->parr,true);
        $this->debug("programme array: $tmp");
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
            $op.="<p class='programmetitle'>" . $this->parr["title"] . "</p>\n";
            $op.="<p class='programmetime'>" . $this->pt->ddt($this->parr["start"]) . " " . $this->displayDuration($this->parr["duration"]) . "</p>\n";
            $op.="<p class='programdesc'>" . $this->parr["description"] . "</p>\n";
            $op.="</div>\n";
            $op.=$this->recordForm();
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
    private function formFields()/*{{{*/
    {
        $fields=array();
        if(isset($this->parr["title"])){
            $arr=array(
                "label"=>"This Title",
                "itype"=>"Radio",
                "name"=>"partsearch",
                "val"=>urlencode("title:" . $this->parr["title"])
            );
            $fields[]=$arr;
        }
        if(isset($this->parr["content"]) && strlen($this->parr["content"])){
            $arr=array(
                "label"=>"This Episode",
                "itype"=>"Radio",
                "name"=>"partsearch",
                "val"=>urlencode("prog:" . $this->parr["content"])
            );
            $fields[]=$arr;
        }
        if(isset($this->parr["series"]) && strlen($this->parr["series"])){
            $arr=array(
                "label"=>"This Series",
                "itype"=>"Radio",
                "name"=>"partsearch",
                "val"=>urlencode("series:" . $this->parr["series"])
            );
            $fields[]=$arr;
        }
        $fields[]=array(
                "label"=>"Like Title",
                "itype"=>"Text",
                "name"=>"liketitlesearch",
                "val"=>""
            );
        $fields[]=array(
                "label"=>"Description",
                "itype"=>"Text",
                "name"=>"likedescsearch",
                "val"=>""
            );
        return $fields;
    }/*}}}*/
    private function recordForm()/*{{{*/
    {
        $op="<div class='recordform'>\n";
        $op.="<form action='" . $_SERVER["PHP_SELF"] . "' method=POST>\n";
        $farr=$this->formFields();
        foreach($farr as $ffield){
            $op.=$this->formRow($ffield["label"],$ffield["itype"],$ffield["name"],$ffield["val"]);
        }
        $op.="<div class='formsubmit'>\n";
        $op.="<input type='submit' value='Record' />\n";
        $op.="</div>\n";
        $op.="</form>\n";
        return $op;
    }/*}}}*/
    private function formRow($label,$inputtype,$name,$val,$class="")/*{{{*/
    {
        $op="<div class='formrow'>\n";
        $op.="<span class='formrowitem right'>\n";
        $op.=$label;
        $op.="</span>\n";
        $op.="<span class='formrowitem left'>\n";
        $op.=strlen($class)?"<input type='$inputtype' name='$name' value='$val' class='$class' />":"<input type='$inputtype' name='$name' value='$val' />\n";
        $op.="</span>\n";
        $op.="</div>\n";
        return $op;
    }/*}}}*/
}
?>
