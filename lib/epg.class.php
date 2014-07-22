<?php

/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * epg.class.php
 *
 * C.C.Allison
 * chris.allison@hotmail.com
 *
 * Started: Sunday 15 June 2014, 09:52:57
 * Last Modified: Tuesday 22 July 2014, 11:27:31
 * Revision: $Id$
 * Version: 0.00
 */

require_once "dvbctrl.class.php";

class EPG extends DVBCtrl
{
    private $epgcapturing=false;
    private $mx=false;
    private $numevents=0;
    private $xml=false;
    private $warningcn=0;
    private $infomap;
    private $dbkeys;
    private $firsteventid=false;
    private $currenteventid=false;
    private $currentevent=false;
    private $dbnotfound=0;
    private $mismatchedtitle=0;
    private $updated=0;
    private $noseriesprogid=0;
    private $dbinserted=0;
    private $dbfailed=0;
    private $ignored=0;
    private $channel;

    public function __construct($logg=false,$host="",$user="",$pass="",$adaptor=0,$dvb=false,$mx=false,$truncate=true,$channel="BBC TWO")/*{{{*/
    {
        parent::__construct($logg,$host,$user,$pass,$adaptor,$dvb);
        if(false===$mx){
            $this->mx=new Mysql($logg,"localhost","tvapp","tvapp","tv");
        }else{
            $this->mx=$mx;
        }
        if($truncate){
            $this->mx->query("truncate epg");
            $this->debug("epg table truncated");
        }
        $this->channel=$channel;
        $this->infomap=array(
            "networkid"=>"ID",
            "mux"=>"Multiplex UID",
            "source"=>"Source",
            "defaultauth"=>"Default Authority",
            "pmtpid"=>"PMT PID",
            "channelname"=>"Name",
            "conditionalaccess"=>"conditionalaccess",
            "type"=>"type"
        );
        $this->dbkeys=array(
            "networkid",
            "mux",
            "source",
            "defaultauth",
            "pmtpid",
            "channelname",
            "conditionalaccess",
            "type"
        );
        $this->freeviewServiceUpdate();
        $this->xml=new XMLReader();
    }/*}}}*/
    public function __destruct()/*{{{*/
    {
        $this->xml=null;
        parent::__destruct();
    }/*}}}*/
    private function rcvEpgEvent()/*{{{*/
    {
        $ret=false;
        $msg="";
        $eventstarted=false;
        $incompleteevent=true;
        if($this->fp){
            while($incompleteevent){
                if($eventstarted){
                    $waitfor="</event>";
                }else{
                    $waitfor="<event ";
                }
                $tmp=fgets($this->fp);
                if(false!==($pos=strpos($tmp,$waitfor))){
                    if($eventstarted){
                        $msg.=$tmp;
                        $eventstarted=false;
                        $incompleteevent=false;
                    }else{
                        $msg=$tmp;
                        $eventstarted=true;
                    }
                }else{
                    if($eventstarted){
                        $msg.=$tmp;
                    }
                }
            }
            $ret=$msg;
        }
        return $ret;
    }/*}}}*/
    private function processTime($str)/*{{{*/
    {
        $tmp=explode(" ",$str);
        $date=trim($tmp[0]);
        $time=trim($tmp[1]);
        $tmp=explode("-",$date);
        $year=intval($tmp[0]);
        $month=intval($tmp[1]);
        $day=intval($tmp[2]);
        $tmp=explode(":",$time);
        $hour=intval($tmp[0]);
        $minute=intval($tmp[1]);
        $second=0;
        $ts=mktime($hour,$minute,$second,$month,$day,$year);
        $lastweek=time() - (7 * 86400);
        if($lastweek<$ts){
            return $ts;
        }else{
            return false;
        }
    }/*}}}*/
    private function processNew()/*{{{*/
    {
        $ret=false;
        $cn=$this->xml->attributeCount;
        if($cn==3){
            if($this->xml->getAttributeNo(2)=="no"){
                if(false===($start=$this->processTime($this->xml->getAttributeNo(0)))){
                    $this->warning("garbled start time for " . $this->xml->getAttributeNo(0));
                }
                if(false===($end=$this->processTime($this->xml->getAttributeNo(1)))){
                    $this->warning("garbled end time for " . $this->xml->getAttributeNo(1));
                }
                if(false!==$start && false!==$end){
                    $ret=array("start"=>$start,"end"=>$end);
                }
            }else{
                $this->debug("encrypted event, ignoring");
            }
        }else{
            $this->warning("incorrect number of attributes for <new> tag, ignoring event");
        }
        return $ret;
    }/*}}}*/
    private function processDetail()/*{{{*/
    {
        $ret=false;
        try{
            $tag=$this->xml->getAttributeNo(1);
            $this->xml->read();
            $value=$this->xml->value;
            $ret=array($tag=>$value);
        }catch(Exception $e){
            $this->warning("Failed to process detail");
            $this->warning($e->getMessage());
        }
        return $ret;
    }/*}}}*/
    private function processXml()/* {{{ */
    {
        $ret=false;
        try{
            $this->xml->read();
            $name=$this->xml->name;
            if("event"==$name){
                $cn=$this->xml->attributeCount;
                if($cn==4){
                    for($x=0;$x<3;$x++){
                        if($x>0){
                            $netid.="." . str_replace("0x","",$this->xml->getAttributeNo($x));
                        }else{
                            $netid=str_replace("0x","",$this->xml->getAttributeNo($x));
                        }
                    }
                    $eventid=str_replace("0x","",$this->xml->getAttributeNo(3));
                    // the event tag has an empty text block which we need to skip past
                    $this->xml->read();
                    $this->xml->read();
                    $tname=$this->xml->name;
                    switch($tname){
                    case "new":
                        if(false!==($arr=$this->processNew())){
                            $ret=array("netid"=>$netid,"eventid"=>$eventid);
                            foreach($arr as $key=>$val){
                                $ret[$key]=$val;
                            }
                        }else{
                            $this->debug("failed to process tag '<new>', ignoring event");
                        }
                        break;
                    case "detail":
                        if(false!==($arr=$this->processDetail())){
                            $ret=array("netid"=>$netid,"eventid"=>$eventid);
                            foreach($arr as $key=>$val){
                                $ret[$key]=$val;
                            }
                        }else{
                            $this->debug("failed to process tag '<detail>', ignoring event");
                        }
                        break;
                    }
                }else{
                    $this->debug("event has $cn attributes, ignoring event");
                }
            }else{
                $this->warning("not an event: this is called $name, ignoring event");
            }
        }catch(Exception $e){
            $this->warning("Cannot read xml");
            $this->warning($e->getMessage());
        }
        return $ret;
    }/* }}} */
    private function sqlSubStr($event)/*{{{*/
    {
        $usql="";
        foreach($event as $key=>$val){
            switch($key){
            case "start":
                $key="starttime";
                break;
            case "end":
                $key="endtime";
                break;
            case "netid":
                $key="networkid";
                break;
            default:
                break;
            }
            if(strlen($usql)){
                $usql.=",$key=" . $this->makeSqlString($val);
            }else{
                $usql="$key=" . $this->makeSqlString($val);
            }
        }
        return $usql;
    }/*}}}*/
    private function updateEPG($event)/*{{{*/
    {
        $sql="select * from epg where eventid='" . $event["eventid"] . "'";
        if(false!==($arr=$this->mx->arrayQuery($sql))){
            $usql=$this->sqlSubStr($event);
            $tsql="update epg set $usql where eventid='" . $event["eventid"] . "'";
            $this->mx->query($tsql);
        }else{
            $usql=$this->sqlSubStr($event);
            $tsql="insert into epg set $usql";
            $this->mx->query($tsql);
            $this->numevents++;
        }
    }/*}}}*/
    private function compareDBtoNet($dbarr,$netarr)/*{{{*/
    {
        $equal=true;
        foreach($this->infomap as $key=>$val){
            if($dbarr[$key]!=$netarr[$val]){
                $equal=false;
                break;
            }
        }
        return $equal;
    }/*}}}*/
    private function makeSqlString($str)/*{{{*/
    {
        $tmp=$str;
        // strip outer quotes
        if('"'==($junk=substr($tmp,0,1))){
            $tmp=substr($tmp,1);
        }
        if('"'==($junk=substr($tmp,strlen($tmp)-1,1))){
            $tmp=substr($tmp,0,strlen($tmp)-1);
        }
        $tmp=$this->mx->escape($tmp);
        return "'$tmp'";
    }/*}}}*/
    private function subSql($sinfo)/*{{{*/
    {
        $sql="";
        foreach($this->dbkeys as $key){
            if($key=="conditionalaccess"){
                $tmp=$key . "=" . $sinfo[$key];
            }else{
                $tmp=$key . "=" . $this->makeSqlString($sinfo[$key]);
            }
            if(strlen($sql)){
                $sql.=", $tmp";
            }else{
                $sql=$tmp;
            }
        }
        return $sql;
    }/*}}}*/
    private function updateChan($sinfo,$lcn,$newchan=false)/*{{{*/
    {
        if($newchan){
            $sql="insert into freeview set id=$lcn, ";
            $esql="";
        }else{
            $sql="update freeview set ";
            $esql=" where id=$lcn";
        }
        $sql.=$this->subSql($sinfo);
        $sql.=$esql;
        $this->mx->query($sql);
    }/*}}}*/
    private function normaliseSinfo($sinfo)/*{{{*/
    {
        foreach($this->infomap as $key=>$val){
            if($key!=="conditionalaccess" && $key!=="type"){
                $sinfo[$key]=$sinfo[$val];
            }
        }
        $sinfo["conditionalaccess"]=$sinfo["Conditional Access?"]=="Free to Air"?0:1;
        switch($sinfo["Type"]){
        case "Data":
            $sinfo["type"]="d";
            break;
        case "Digital Radio":
            $sinfo["type"]="r";
            break;
        default:
            $sinfo["type"]="t";
            break;
        }
        return $sinfo;
    }/*}}}*/
    private function freeviewServiceUpdate()/*{{{*/
    {
        $ret=false;
        $newchans=0;
        $updatechans=0;
        $samechans=0;
        $this->info("updating services");
        if(false!==($lcns=$this->lsServices())){
            if(false!==($cn=$this->ValidArray($lcns))){
                $this->info("$cn services to check");
                foreach($lcns as $lcn=>$channelname){
                    if(false!==($sinfo=$this->serviceInfo($channelname))){
                        $sinfo=$this->normaliseSinfo($sinfo);
                        $sql="select * from freeview where id=$lcn";
                        if(false!==($arr=$this->mx->arrayQuery($sql))){
                            if(false===($junk=$this->compareDBtoNet($arr[0],$sinfo))){
                                $this->updateChan($sinfo,$lcn);
                                $updatechans++;
                            }else{
                                $samechans++;
                            }
                        }else{
                            $this->info("New channel: " . $sinfo["Name"]);
                            $this->updateChan($sinfo,$lcn,true);
                            $newchans++;
                        }
                    }else{
                        $this->warning("Failed to retreive info for $lcn: $channelname");
                    }
                }
            }else{
                $this->warning("No services from dvbstreamer");
            }
        }else{
            $this->warning("failed to retrieive services from dvbstreamer");
        }
        $this->info("Freeview Update: $newchans new, $updatechans updated, $samechans remain the same");
        return $ret;
    }/*}}}*/
    private function processEvent($event)/*{{{*/
    {
        if(false===$this->currenteventid){
            $this->currentevent=$event;
            $this->currenteventid=$event["eventid"];
        }elseif($this->currenteventid==$event["eventid"]){
            $this->currentevent=array_merge($this->currentevent,$event);
        }else{
            if(false===$this->firsteventid){
                // ignore the very first full event received as it may be incomplete
                // make a note of the next eventid so that we can stop when we see it again
                $this->firsteventid=$event["eventid"];
                $this->currentevent=$event;
                $this->currenteventid=$event["eventid"];
            }else{
                $this->updateDB();
                $this->numevents++;
                $this->currentevent=$event;
                $this->currenteventid=$event["eventid"];
            }
        }
    }/*}}}*/
    private function insertEventIntoDB()/*{{{*/
    {
        $rtid=0;
        $sql="select rtid from channel where freeviewid = (select id from freeview where networkid='" . $this->currentevent["netid"] . "')";
        if(false!==($arr=$this->mx->arrayQuery($sql))){
            if(isset($arr[0]["rtid"])){
                $rtid=$arr[0]["rtid"];
            }
        }
        $hsql="insert into schedule (channel,title,description,starttime,endtime";
        $sql=$rtid . ",";
        $sql.=$this->makeSqlString($this->currentevent["title"]) . ",";
        $sql.=$this->makeSqlString($this->currentevent["description"]) . ",";
        $sql.=$this->currentevent["start"] . ",";
        $sql.=$this->currentevent["end"];
        if(isset($this->currentevent["content"])){
            $sql.=",'" . $this->currentevent["content"] . "'";
            $hsql.=",programid";
        }
        if(isset($this->currentevent["series"])){
            $sql.=",'" . $this->currentevent["series"] . "'";
            $hsql.=",seriesid";
        }
        $hsql.=") values ($sql)";
        $this->mx->query($hsql);
        $this->dbinserted++;
    }/*}}}*/
    private function insertEventIntoEPG()/*{{{*/
    {
        $fid=0;
        $sql="select id from freeview where networkid='" . $this->currentevent["netid"] . "'";
        if(false!==($arr=$this->mx->arrayQuery($sql))){
            if(isset($arr[0]["id"])){
                $fid=$arr[0]["id"];
            }
        }
        if($fid>0){
            $hsql="insert into epg (networkid,starttime,endtime,title,description";
            $sql=$this->makeSqlString($this->currentevent["netid"]) . ",";
            $sql.=$this->currentevent["start"] . ",";
            $sql.=$this->currentevent["end"];
            $sql.=$this->makeSqlString($this->currentevent["title"]) . ",";
            $sql.=$this->makeSqlString($this->currentevent["description"]) . ",";
            if(false!==($cn=$this->testArrayMember($this->currentevent,"content"))){
                $sql.=",'" . $this->currentevent["content"] . "'";
                $hsql.=",programid";
            }
            if(false!==($cn=$this->testArrayMember($this->currentevent,"series"))){
                $sql.=",'" . $this->currentevent["series"] . "'";
                $hsql.=",seriesid";
            }
            $hsql.=") values ($sql)";
            $this->mx->query($hsql);
            $this->dbinserted++;
        }else{
            $this->dbfailed++;
        }
    }/*}}}*/
    private function updateDB()/*{{{*/
    {
        $sql="select networkid from freeview,channel where channel.getdata=1 and channel.freeviewid=freeview.id and freeview.networkid='" . $this->currentevent["netid"] . "'";
        if(false!==($arr=$this->mx->arrayQuery($sql))){
            $sql="select * from schedule where channel = (select rtid from channel where freeviewid = (select id from freeview where networkid='" . $this->currentevent["netid"] . "')) and starttime = " . $this->currentevent["start"] . " and endtime = " . $this->currentevent["end"];
            if(false!==($arr=$this->mx->arrayQuery($sql))){
                if(0==($cn=$this->ValidArray($arr))){
                    // event doesn't yet exist in db
                    // so insert it
                    $this->insertEventIntoEPG();
                    // $this->insertEventIntoDB();
                    $this->dbnotfound++;
                }else{
                    if(isset($arr[0]["title"])){
                        if($arr[0]["title"]!==$this->currentevent["title"]){
                            $this->debug("titles do not match - not updating db: " . $arr[0]["title"] . " and " . $this->currentevent["title"]);
                            $this->debug("at " . $this->currentevent["start"] . " on " . $this->currentevent["netid"]);
                            $this->mismatchedtitle++;
                        }else{
                            $sql="update schedule set ";
                            $esql=" where id=" . $arr[0]["id"];
                            $ssql="";
                            if(isset($this->currentevent["content"]) && strlen($this->currentevent["content"]) && $arr[0]["programid"]!=$this->currentevent["content"]){
                                //$sql="update schedule set programid='" . $this->currentevent["content"] . "' where id=" . $arr[0]["id"];
                                $ssql="programid='" . $this->currentevent["content"] . "'";
                                // $this->mx->query($sql);
                            }
                            if(isset($this->currentevent["series"]) && strlen($this->currentevent["series"]) && $arr[0]["seriesid"]!=$this->currentevent["series"]){
                                // $sql="update schedule set seriesid='" . $this->currentevent["series"] . "' where id=" . $arr[0]["id"];
                                // $this->mx->query($sql);
                                if(strlen($ssql)){
                                    $ssql.=", seriesid='" . $this->currentevent["series"] . "'";
                                }else{
                                    $ssql="seriesid='" . $this->currentevent["series"] . "'";
                                }
                            }
                            if(strlen($ssql)){
                                $this->mx->query($sql . $ssql . $esql);
                                $this->updated++;
                            }else{
                                $this->noseriesprogid++;
                            }
                        }
                    }else{
                        $tmp=print_r($arr,true);
                        $this->debug("array title not set");
                        $this->debug($tmp);
                    }
                }
            }else{
                $this->insertEventIntoDB();
                $this->dbnotfound++;
            }
        }else{
            $this->ignored++;
        }
    }/*}}}*/
    public function getNumEvents()/*{{{*/
    {
        return $this->numevents;
    }/*}}}*/
    public function epgCapStart()/*{{{*/
    {
        $srv=$this->channel;
        $nottuned=true;
        // stop capturing if it is currently running
        // and ignore any errors if this fails
        $this->request("epgcapstop");
        if(false===($currrecs=$this->lsRecording(true))){
            if(false!==($srvs=$this->lsServices())){
                if(false!==($cn=$this->ValidArray($srvs))){
                    $srv=$srvs[0];
                }else{
                    $this->warning("No services returned from request for service list");
                }
            }else{
                $this->warning("failed to retrieve service list");
            }
        }else{
            if(false!==($cn=$this->ValidArray($currrecs))){
                if($cn){
                    $this->debug("already tuned, not selecting a channel");
                    $nottuned=false;
                }
            }
        }
        if($nottuned){
            $this->debug("Selecting $srv as EPG capture channel");
            $this->select($srv);
        }
        if(false!==($junk=$this->request("epgcapstart"))){
            $this->epgcapturing=true;
            $this->debug("Starting to capture EPG data");
            $this->sendData("epgdata");
            return true;
        }
        return false;
    }/*}}}*/
    public function epgCapStop()/*{{{*/
    {
        // this may not work as dvbstreamer may ignore it
        if(false!==($junk=$this->request("epgcapstop"))){
            $this->epgcapturing=false;
            $this->disconnect();
            // $this->connect();
        }else{
            // disconnect anyway, then dvbstreamer knows 
            // we don't want anymore data
            $this->disconnect();
        }
        return true;
    }/*}}}*/
    public function epgEvent()/*{{{*/
    {
        $ret=false;
        $currentnumevents=$this->numevents;
        while($currentnumevents==$this->numevents){
            if(false!==($eventstr=$this->rcvEpgEvent())){
                try{
                    $this->xml->xml($eventstr);
                    $this->warningcn=0;
                    if(false!==($event=$this->processXml())){
                        // $this->updateEPG($event);
                        $this->processEvent($event);
                        if($this->firsteventid==$this->currenteventid && $this->numevents>1){
                            $this->info("First event received again");
                            $ret=array("numevents"=>$this->numevents,"dbnotfound"=>$this->dbnotfound,"mismatchedtitle"=>$this->mismatchedtitle,"updated"=>$this->updated,"noseriesprogid"=>$this->noseriesprogid,"dbinserted"=>$this->dbinserted,"ignored"=>$this->ignored,"stopnow"=>true,"dbfailed"=>$this->dbfailed);
                        }else{
                            $ret=array("numevents"=>$this->numevents,"dbnotfound"=>$this->dbnotfound,"mismatchedtitle"=>$this->mismatchedtitle,"updated"=>$this->updated,"noseriesprogid"=>$this->noseriesprogid,"dbinserted"=>$this->dbinserted,"ignored"=>$this->ignored,"stopnow"=>false,"dbfailed"=>$this->dbfailed);
                        }
                        // $ret=$this->numevents;
                    }
                }catch(Exception $e){
                    $this->warning("failed to interpret string as valid xml");
                    $this->warning($e->getMessage());
                    $this->warningcn++;
                    if($this->warningcn>9){
                        $this->error("10 straight failed xml interpretations");
                        $this->epgCapStop();
                    }
                }
            }
        }
        return $ret;
    }/*}}}*/
}
?>
