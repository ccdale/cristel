<?php
/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * data.class.php
 *
 * Started: Saturday  8 August 2015, 08:23:32
 * Last Modified: Sunday  9 August 2015, 15:21:33
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

class Data extends Base
{
    protected $cx=false;
    protected $table=false;
    protected $columns=false;
    protected $keyname=false;
    protected $keyvalue=0;
    protected $arr=false;
    protected $dirty=false;

    public function __construct($logg=false,$cx=false,$table=false,$columns=false,$keyname=false,$keyvalue=0)/*{{{*/
    {
        parent::__construct($logg);
        $this->cx=$cx;
        $this->table=$table;
        $this->columns=$columns;
        $this->keyname=$keyname;
        $this->keyvalue=$keyvalue;
        $this->getrow();
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
    public function getField($field)/*{{{*/
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
    public function amdirty()/*{{{*/
    {
        return $this->dirty;
    }/*}}}*/
    public function updatedb()/*{{{*/
    {
        return $this->setrow();
    }/*}}}*/
    public function getarr()/*{{{*/
    {
        if(false===$this->arr){
            $this->getrow();
        }
        return $this->arr;
    }/*}}}*/
    protected function getrow()/*{{{*/
    {
        if((false!==$this->cx) && (false!==$this->keyname) && (false!==$this->keyvalue)){
            $sql="select * from ";
            $sql.=$this->table;
            $sql.=" where ";
            $sql.=$this->keyname;
            $sql.="='";
            $sql.=$this->keyvalue;
            $sql.="'";
            $tmp=$this->cx->arrayQuery($sql);
            $this->arr=$tmp[0];
            $this->dirty=false;
        }
    }/*}}}*/
    protected function setrow()/*{{{*/
    {
        if($this->dirty){
            if($this->checkarray()){
                if(false!==$this->cx){
                    $sql=$this->getupdatesql();
                    if(false!==($junk=$this->cx->query($sql))){
                        $this->dirty=false;
                    }
                }
            }
        }
        return !$this->dirty;
    }/*}}}*/
    protected function checkarray()/*{{{*/
    {
        $ret=false;
        if(false!==($cn=$this->ValidArray($this->arr))){
            if(false!==($cn=$this->ValidArray($this->columns))){
                $failed=false;
                foreach($this->columns as $column){
                    if(!isset($this->arr[$column])){
                        $failed=true;
                        break;
                    }
                }
                if(!$failed){
                    $ret=true;
                }
            }
        }
        return $ret;
    }/*}}}*/
    protected function getupdatesql()/*{{{*/
    {
        $sql="insert or replace into ";
        $sql.=$this->table;
        $sql.=" (";
        foreach($this->columns as $column){
            $sql.=$column . ",";
        }
        $sql=substr($sql,0,-1);
        $sql.=") values (";
        foreach($this->columns as $column){
            $sql.=$this->arr[$column] . ",";
        }
        $sql=substr($sql,0,-1);
        $sql.=")";
        return $sql;
    }/*}}}*/
}
?>
