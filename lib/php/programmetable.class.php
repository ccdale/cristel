<?php
/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * programmetable.class.php
 *
 * Started: Sunday 16 August 2015, 04:46:39
 * Last Modified: Sunday 16 August 2015, 11:45:47
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

class ProgrammeTable extends Base
{
    public function __construct($logg=false)/*{{{*/
    {
        parent::__construct($logg);
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
    public function linkToSelf($name,$args=false,$class="",$title="")/*{{{*/
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
    public function makeAttsString($atts=false)/*{{{*/
    {
        $op="";
        if(false!==($cn=$this->ValidArray($atts)) && $cn>0){
            foreach($atts as $key=>$val){
                $op.=$key . "=\"" . $val . "\" ";
            }
            $op=substr($op,0,-1);
        }elseif(false!==($cn=$this->ValidString($atts)) && $cn>0){
            $op=$atts;
        }
        return $op;
    }/*}}}*/
    public function ddt($ts)/*{{{*/
    {
        return $this->displayDay($ts) . " " . $this->displayTime($ts);
    }/*}}}*/
    public function displayDay($ts=0)/*{{{*/
    {
        if($ts){
            $op=date("D d",$ts);
        }else{
            $op=date("D d");
        }
        return $op;
    }/*}}}*/
    public function displayDM($ts=0)/*{{{*/
    {
        if($ts>0){
            $op=date("D d M",$ts);
        }else{
            $op=date("D d M");
        }
        return $op;
    }/*}}}*/
    public function displayTime($ts=0)/*{{{*/
    {
        if($ts){
            $op=date("H:i",$ts);
        }else{
            $op=date("H:i");
        }
        return $op;
    }/*}}}*/
    public function Table($atts=false,$content="")/*{{{*/
    {
        return $this->makeTag("table",$atts,$content);
    }/*}}}*/
    public function tableRow($atts=false,$content="")/*{{{*/
    {
        return $this->makeTag("tr",$atts,$content);
    }/*}}}*/
    public function tableData($atts=false,$content="")/*{{{*/
    {
        return $this->makeTag("td",$atts,$content);
    }/*}}}*/
    public function tableHead($atts=false,$content="")/*{{{*/
    {
        return $this->makeTag("th",$atts,$content);
    }/*}}}*/
    private function makeTag($tag,$atts=false,$content="")/*{{{*/
    {
        $op="<" . $tag . " ";
        $op.=$this->makeAttsString($atts);
        $op=rtrim($op);
        $op.=">\n";
        $op.=$content;
        $op.="</" . $tag . ">\n";
        return $op;
    }/*}}}*/
}

?>
