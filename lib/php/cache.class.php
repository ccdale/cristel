<?php
/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * cache.class.php
 *
 * Started: Saturday 21 December 2013, 03:19:03
 * Last Modified: Thursday 24 July 2014, 20:44:25
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

require_once "base.class.php";

class Cache extends Base
{
    private $cachename;
    private $cachedir;
    private $dirok=false;

    public function __construct($logg=false,$cachename="phpcache")/*{{{*/
    {
        parent::__construct($logg);
        $this->cachename=$cachename;
        $home=getenv("HOME");
        $this->cachedir=$this->unixPath($home) . ".cache/" . $this->cachename;
        if($this->checkFile($this->cachedir,false)){
            if(false==($junk=mkdir($this->cachedir,0700,true))){
                $this->error("Unable to create cache directory: " . $this->cachedir);
                $this->dirok=false;
            }else{
                $this->dirok=true;
            }
        }else{
            $this->dirok=true;
        }
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
    public function getCacheFile($fn)/*{{{*/
    {
        $ret=false;
        if(false!=($cfn=$this->makeCacheFileName($fn,true))){
            $ret=unserialize(file_get_contents($cfn));
        }
        return $ret;
    }/*}}}*/
    public function setCacheFile($fn,$data)/*{{{*/
    {
        $ret=false;
        if(false!=($cfn=$this->makeCacheFileName($fn))){
            $ret=file_put_contents($cfn,serialize($data));
        }
        return $ret;
    }/*}}}*/
    public function getCacheFileTime($fn)/*{{{*/
    {
        $ret=false;
        if(false!=($cfn=$this->makeCacheFileName($fn,true))){
            $ret=filemtime($cfn);
        }
        return $ret;
    }/*}}}*/
    public function cacheExists($fn)/*{{{*/
    {
        if(false!=($junk=$this->makeCacheFileName($fn,true))){
            return true;
        }else{
            return false;
        }
    }/*}}}*/
    private function makeCacheFileName($fn,$exists=false)/*{{{*/
    {
        $ret=false;
        if($this->dirok){
            $fn="$fn";
            if($this->ValidStr($fn)){
                $tmpfn=$this->unixPath($this->cachedir) . $fn;
                if($exists){
                    if(false!=($ret=$this->checkFile($tmpfn,true))){
                        $ret=$tmpfn;
                    }
                }else{
                    $ret=$tmpfn;
                }
            }
        }
        return $ret;
    }/*}}}*/
}
?>
