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
 * Last Modified: Wednesday 16 July 2014, 05:00:58
 * Revision: $Id$
 * Version: 0.00
 */

require_once "base.class.php";
require_once "logging.class.php";
require_once "cli.class.php"; // required by logging class
require_once "simple_mysql.class.php";

class Cristel extends Base
{
    private $dbhost;
    private $dbuser;
    private $dbpass;
    private $dbname;
    public $logg;
    public $mx;

    public function __construct($dbpass,$loglevel=LOG_INFO,$toconsole=true,$logident="CRISTEL",$dbhost="localhost",$dbuser="tvd",$dbname="tv")/*{{{*/
    {
        $this->dbpass=$dbpass;
        $this->dbuser=$dbuser;
        $this->dbhost=$dbhost;
        $this->dbname=$dbname;
        $this->logg=new Logging($toconsole,$logident,0,$loglevel,false,false);
        parent::__construct($this->logg);
        $this->debug("Cristel instantiating");
        $this->mx=new MySql($this->logg,$this->dbhost,$this->dbuser,$this->dbpass,$this->dbname);
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        $this->mx=null;
        parent::__destruct();
        $this->logg=null;
    }/*}}}*/
}
?>
