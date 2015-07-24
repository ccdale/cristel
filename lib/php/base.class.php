<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 * 
 * base.class.php
 *
 * Started: Friday 24 May 2013, 23:41:08
 * Last Modified: Thursday 24 July 2014, 20:43:23
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

class Base
{
    private $log=false;
    private $canlog=false;

    public function __construct($log=false)/*{{{*/
    {
        if($log && is_object($log) && get_class($log)=="Logging"){
            $this->log=$log;
            $this->canlog=true;
        }
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
    }/*}}}*/
    public function logg($msg,$level=LOG_INFO) /*{{{*/
    {
        if($this->canlog){
            $this->log->message($msg,$level);
        }
    }/*}}}*/
    public function ValidFile($fqf)/*{{{*/
    {
        $ret=false;
        if($this->ValidStr($fqf)){
            if(file_exists($fqf)){
                $ret=true;
            }
        }
        return $ret;
    }/*}}}*/
    public function ValidString($str)/*{{{*/
    {
        return $this->ValidStr($str);
    }/*}}}*/
    public function ValidStr($str) /*{{{*/
    {
        if(is_string($str)){
            return strlen($str);
        }else{
            return false;
        }
    }/*}}}*/
    public function ValidArray($arr)/*{{{*/
    {
        if(is_array($arr)){
            return count($arr);
        }else{
            return false;
        }
    }/*}}}*/
    public function testArrayMember($arr,$member)/*{{{*/
    {
        $ret=false;
        if(false!==($junk=$this->ValidArray($arr))){
            if(false!=($cn=$this->ValidString($member))){
                if($cn>0){
                    if(isset($arr[$member])){
                        $ret=$this->ValidString($arr[$member]);
                    }
                }
            }
        }
        return $ret;
    }/*}}}*/
    public function testArrayMemberInt($arr,$member)/*{{{*/
    {
        $ret=false;
        if(false!==($junk=$this->ValidArray($arr))){
            $junk=intval($member);
            $junk+=0;
            if($junk===$member){
                if(isset($arr[$member])){
                    $ret=$this->ValidString($arr[$member]);
                }
            }
        }
        return $ret;
    }/*}}}*/
    public function hmsToSec($hms) // {{{
    {
        if($this->ValidStr($hms)){
            $i=0;
            if(strpos($hms,".")!==false){
                $tarr=explode(".",$hms);
                $ii=intval($tarr[1]);
                if($ii>499){
                    $i=1;
                }
            }
            $arr=explode(":",$hms);
            $cn=0;
            if(is_array($arr) && (3==($cn=count($arr)))){
                return ($arr[0]*3600)+($arr[1]*60)+$arr[2]+$i;
            }elseif(2==$cn){
                return ($arr[0]*60)+$arr[1]+$i;
            }else{
                return "";
            }
        }else{
            return false;
        }
    } // }}}
    public function secToHMS($sec,$showdays=false) // {{{
    {
        $days=0;
        if($showdays){
            $days=intval($sec/86400);
            $sec=$sec%86400;
        }
        $hrs=intval($sec/3600);
        $rem=$sec%3600;
        $mins=intval($rem/60);
        $rem=$rem%60;
        if($days==1){
            $daysstring="day";
        }else{
            $daysstring="days";
        }
        if($showdays){
            $tmp=sprintf("%d $daysstring, %02d:%02d:%02d",$days,$hrs,$mins,$rem);
        }else{
            $tmp=sprintf("%02d:%02d:%02d",$hrs,$mins,$rem);
        }
        return $tmp;
    } // }}}
    private function loghelper($msg,$level)/*{{{*/
    {
        if($level==LOG_DEBUG){
            $class=get_class($this);
            $msg="Class " . $class . ": " . $msg;
        }
        $this->logg($msg,$level);
    }/*}}}*/
    protected function info($msg) // {{{
    {
        $this->loghelper($msg,LOG_INFO);
    } // }}}
    protected function debug($msg) // {{{
    {
        $this->loghelper($msg,LOG_DEBUG);
    } // }}}
    protected function notice($msg)/*{{{*/
    {
        $this->loghelper($msg,LOG_NOTICE);
    }/*}}}*/
    protected function warn($msg) // {{{
    {
        $this->loghelper($msg,LOG_WARNING);
    } // }}}
    protected function warning($msg) // {{{
    {
        $this->loghelper($msg,LOG_WARNING);
    } // }}}
    protected function error($msg) // {{{
    {
        $this->loghelper($msg,LOG_ERR);
    } // }}}
    /** unixPath {{{
     * ensures that $path is in the correct unix format for a directory
     *
     * changes backslash "\" path identifiers to forwardslash (windows->unix)
     * adds a trailing backslash if necessary.
     * 
     * @param mixed $path 
     * @access public
     * @return string
     */
    public function unixPath($path)
    {
        $tpath=str_replace(chr(92),'/',$path);
        if(substr($tpath,-1)=="/"){
            return $path;
        }else{
            return $path . "/";
        }
    } /*}}}*/
    /** checkFile {{{
     * checks for the existance or non-existance of file(s).
     *
     * checks if file $fn exists or doesn't exist depending on
     * the $exists parameter (true || false).
     * If file $fn is an array, the operation is performed on each
     * member of the array, with the results anded together.
     * 
     * @param array|string $fn 
     * @param bool $exists 
     * @access public
     * @return bool
     */
    public function checkFile($fn="",$exists=true)
    {
        $ret=false;
        if(is_string($fn) && strlen($fn)){
            $ret=$exists?file_exists($fn):!file_exists($fn);
        }elseif(is_array($fn)){
            $tmp=true;
            $c=count($fn);
            for($x=0;$x<$c;$x++){
                $tmp=$this->checkFile($fn[$x],$exists) && $tmp;
            }
            $ret=$tmp;
        }
        return $ret;
    } /*}}}*/
}
?>
