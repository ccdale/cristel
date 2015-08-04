<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * channel.class.php
 *
 * Started: Tuesday  4 August 2015, 05:42:21
 * Last Modified: Tuesday  4 August 2015, 06:09:48
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

require_once "base.class.php";

class Channel extends Base
{
    private $cx=false;
    private $arr=false;
    private $dirty=false;

    public function __construct($logg=false,$cx=false,$source=false,$name=false)/*{{{*/
    {
        parent::__construct($logg);
        $this->cx=$cx;
        $this->arr=array("source"=>$source,"name"=>$name);
        $this->getrow();
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
    public function visiblechans()/*{{{*/
    {
        $varr=false;
        if(false!==$this->cx){
            $sql="select * from channels where visible=1 order by logicalid asc";
            $varr=$this->cx->arrayQuery($sql);
        }
        return $varr;
    }/*}}}*/
    public function getField($field="muxid")/*{{{*/
    {
        if(isset($this->arr[$field])){
            return $this->arr[$field];
        }else{
            return false;
        }
    }/*}}}*/
    public function setField($field,$value)/*{{{*/
    {
        $this->arr["field"]=$value;
        $this->dirty=true;
    }/*}}}*/
    private function getrow()/*{{{*/
    {
        if(false!==$this->cx){
            $sql=false;
            if(false!==$this->arr["source"]){
                $sql="select * from channels where source='" . $this->arr["source"] . "' limit 1";
            }elseif(false!==$this->arr["name"]){
                $sql="select * from channels where name='" . $this->arr["name"] . "' limit 1";
            }
            if($sql){
                $tmp=$this->cx->arrayQuery($sql);
                $this->arr=$tmp[0];
                $this->dirty=false;
            }
        }
    }/*}}}*/
}
?>
