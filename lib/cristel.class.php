<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * cristel.class.php
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Wednesday 16 July 2014, 04:36:37
 * Last Modified: Wednesday 16 July 2014, 11:42:31
 * Revision: $Id$
 * Version: 0.00
 */

require_once "base.class.php";
require_once "logging.class.php";
require_once "cli.class.php"; // required by logging class
require_once "simple-mysql.class.php";

class Cristel extends Base
{
    private $dbhost;
    private $dbuser;
    private $dbpass;
    private $dbname;
    public $logg;
    public $mx;
    private $jobs=false;

    public function __construct($dbpass,$loglevel=LOG_INFO,$toconsole=true,$logident="CRISTEL",$dbhost="localhost",$dbuser="tvapp",$dbname="tv")/*{{{*/
    {
        $this->dbpass=$dbpass;
        $this->dbuser=$dbuser;
        $this->dbhost=$dbhost;
        $this->dbname=$dbname;
        $this->logg=new Logging($toconsole,$logident,0,$loglevel,false,false);
        parent::__construct($this->logg);
        $this->debug("Cristel instantiating");
        $this->mx=new MySql($this->logg,$this->dbhost,$this->dbuser,$this->dbpass,$this->dbname);
        if($this->mx->amOK()){
            $this->debug("Connected to db ok");
        }
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        $this->debug("Cristel is shutting down");
        $this->debug("Closing db connections");
        $this->mx=null;
        $this->debug("Killing parent base class");
        parent::__destruct();
        $this->logg->debug("killing log class");
        $this->logg=null;
    }/*}}}*/
    private function run()/*{{{*/
    {
    }/*}}}*/
    private function initialise()/*{{{*/
    {
        $this->initialiseJobs();
    }/*}}}*/
    private function initialiseJobs()/*{{{*/
    {
        if(false!==($jstr=$this->getConfVal("jobs"))){
            $this->jobs=unserialize($jstr);
        }else{
            $this->jobs=array();
            $this->addJob("epg",time(),array("priority"=>10));
        }
    }/*}}}*/
    private function addJob($jobname,$when,$data=false)/*{{{*/
    {
        $this->jobs[]=array("name"=>$jobname,"when"=>$when,"data"=>$data);
    }/*}}}*/
    private function getConfVal($confname,$default=false)/*{{{*/
    {
        $ret=$default;
        if(false!==($cn=$this->ValidString($confname))){
            if(false!==($arr=$this->mx->arrayQuery("select confval from system where confname='$confname'"))){
                if(false!==($cn=$this->ValidArray($arr))){
                    if($cn>0){
                        if(isset($arr[0][$confname])){
                            $ret=$arr[0][$confname];
                        }
                    }
                }
            }
        }
        return $ret;
    }/*}}}*/
    private function setConfVal($confname,$confval)/*{{{*/
    {
        if(false!==($arr=$this->mx->arrayQuery("select id from system where confname='$confname'"))){
            $id=$arr[0]["id"];
            $sql="update system set confval='$confval' where id=$id";
        }else{
            $sql="insert into system (confname,confval) values ('$confname','$confval')";
        }
        $this->mx->query($sql);
    }/*}}}*/
}
?>
