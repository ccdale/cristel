<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * recorder.class.php
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Saturday 19 July 2014, 12:46:33
 * Last Modified: Saturday 19 July 2014, 12:49:36
 * Revision: $Id$
 * Version: 0.00
 */
require_once "base.class.php";

class Recorder extends Base
{
    private $mx=false;

    public function __construct($logg=false,$mx=false)/*{{{*/
    {
        parent::__construct($logg);
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        parent::__destruct();
    }/*}}}*/
}
?>
