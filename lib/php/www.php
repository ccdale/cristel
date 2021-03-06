<?php
/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * www.php
 *
 * Started: Saturday  8 August 2015, 08:23:32
 * Last Modified: Sunday  9 August 2015, 13:55:51
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

/** // {{{ funtion GP($var,$trusted=false)
* returns the contents of the GET or POST variable
* favours POST over GET
*/
function GP($var,$trusted=false)
{
    $op=false;
    if(isset($_GET[$var])){
        $op=$_GET[$var];
    }
	if(isset($_POST[$var]))
	{
		$op=$_POST[$var];
	}
	if(is_string($op))
	{
        $op=trim(urldecode($op));
        if(!$trusted){
            $op=htmlspecialchars($op,ENT_QUOTES);
        }
		// $op=addslashes($op);
	}
	return $op;
} // }}}
/** // {{{ function getDefaultInt($var,$default)
 * returns the contents of the GET or POST variable
 * as an int, or the default value if it isn't set
 */
function getDefaultInt($var,$default)
{
    $op=false;
    if(false!==($tmp=GP($var))){
        $tmp=intval($tmp);
        if($tmp>0){
            $op=$tmp;
        }else{
            $op=$default;
        }
    }else{
        $op=$default;
    }
    return $op;
} // }}}
/* // {{{ function GPA($namearr)
 * returns an array containing 
 * selected entries from the $_GET and $_POST arrays
 * the selected keys are in $namearr
 */
function GPA($namearr)
{
    $arr=false;
    if(is_array($namearr) && count($namearr))
    {
        reset($namearr);
        while(list(,$v)=each($namearr))
        {
            $arr[$v]=GP($v);
        }
    }else{
        if(is_string($namearr))
        {
            $arr[$namearr]=GP($namearr);
        }
    }
    return $arr;
} // }}}
function GPAll($trusted=false) // {{{
{
    $op=array();
    $arr=array_merge($_GET,$_POST);
    foreach($arr as $key=>$val){
        if(is_string($val)){
            $op[$key]=trim(urldecode($val));
            if(!$trusted){
                $op[$key]=htmlspecialchars($op[$key],ENT_QUOTES);
            }
        }
    }
    return $op;
} // }}}
function GPType($var,$type="string",$trusted=false) /*{{{*/
{
    $op=GP($var,$trusted);
    switch($type){
    case "int":
        $op=intval($op);
        break;
    case "string":
        $op="$op";
    }
    return $op;
} /*}}}*/
?>
