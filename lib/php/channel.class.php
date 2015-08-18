<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * channel.class.php
 *
 * Started: Tuesday  4 August 2015, 05:42:21
 * Last Modified: Sunday 16 August 2015, 04:22:38
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

require_once "data.class.php";

class Channel extends Data
{
    private $collist=array("source","name","priority","visible","favourite","logicalid","muxid");

    public function __construct($logg=false,$cx=false,$logicalid=false,$source=false,$name=false)/*{{{*/
    {
        $keyname=false;
        $keyvalue=0;
        if(false!==$source){
            $keyname="source";
            $keyvalue=$source;
        }elseif(false!==$name){
            $keyname="name";
            $keyvalue=$name;
        }elseif(false!==$logicalid){
            $keyname="logicalid";
            $keyvalue=$logicalid;
        }
        parent::__construct($logg,$cx,"Channels",$this->collist,$keyname,$keyvalue);
        // $this->getrow();
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
    public function visiblechans()/*{{{*/
    {
        $rarr=false;
        if(false!==$this->cx){
            $sql="select * from Channels where visible=1 order by logicalid asc";
            $varr=$this->cx->arrayQuery($sql);
            if(false!==($cn=$this->ValidArray($varr))){
                if($cn>0){
                    $this->debug("found $cn channels");
                    $rarr=array();
                    foreach($varr as $v){
                        $rarr[$v["logicalid"]]=$v;
                    }
                }else{
                    $this->warning("0 visible channels found");
                }
            }else{
                $this->warning("No data returned for visible channels");
            }
        }else{
            $this->warning("database connector not set");
        }
        return $rarr;
    }/*}}}*/
}
?>
