#!/usr/bin/env php
<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * simple-sqlite.class.php
 *
 * Started: Sunday  2 August 2015, 12:29:49
 * Last Modified: Sunday  2 August 2015, 12:57:11
 */
require_once "base.class.php"

/** SSql Class
 * simple class to do db stuff with sqlite3 databases
 */

class SSql extends Base
{
    private $db=false;
    private $connected=false;
    private $rs=false;

    public function __construct($fqfn=false,$logg=false)/*{{{*/
    {
        parent::__construct($logg);
        if(false!==$fqfn){
            if(file_exists($fqfn)){
                try {
                    $this->db=new SQLite3($fqfn);
                    $this->connected=true;
                }catch (Exception $e){
                    $this->error($e->getMessage());
                }
            }else{
                $this->error("DB file does not exist: $fqfn");
            }
        }
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
        if($this->amOK()){
            $this->db->close();
            $this->connected=false;
        }
    }/*}}}*/
    public function amOK()/*{{{*/
    {
        return $this->connected;
    }/*}}}*/
    public function query($sql)/*{{{*/
    {
        $this->rs=false;
        if($this->amOK() && $this->ValidStr($sql)){
            $this->debug("Query: $sql");
            try{
                $this->rs=$this->db->query($sql);
            }catch (Exception $e){
                $this->error("Query Error: " . $e->getMessage());
                $this->error("Query: $sql");
            }
        }else{
            $this->warning("Not connected or invalid query string");
            $tmp=print_r($sql,true);
            $this->warning("print_r(\$sql): $tmp");
        }
        return $this->rs;
    }/*}}}*/
    public function insertQuery($sql="") // {{{
    {
        /*
         * returns insert id or false for insert queries
         */
        $ret=$this->query($sql);
        if($ret){
            $ret=$this->db->lastInsertRowID();
        }
        return $ret;
    } // }}}
    public function deleteQuery($sql="")/*{{{*/
    {
        /*
         * returns the number of rows deleted or false for delete queries
         */
        $ret=$this->query($sql);
        if($ret){
            $ret=$this->db->changes();
        }
        return $ret;
    }/*}}}*/
    public function arrayQuery($sql="") // {{{
    {
        $ret=false;
        $this->query($sql);
        if($this->rs){
            $cn=$this->rs->numRows();
            $this->debug("$cn rows returned");
            if($cn>0){
                $ret=array();
                while(false!=($arr=$this->rs->fetchArray(SQLITE3_ASSOC))){
                    $ret[]=$arr;
                }
            }
        }
        return $ret;
    } // }}}
    public function escape($str="")/*{{{*/
    {
        return sqlite_escape_string($str);
    }/*}}}*/
}

?>
