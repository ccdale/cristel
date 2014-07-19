<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * cache.class.php
 *
 * Started: Saturday 21 December 2013, 03:19:03
 * Last Modified: Friday 18 July 2014, 10:13:22
 * Revision: $Id$
 * Version: 0.00
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
