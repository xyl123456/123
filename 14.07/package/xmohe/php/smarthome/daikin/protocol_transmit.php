<?php

#include 'foundation/crc16.php';

require_once('foundation/crc16.php');

class DaikinProtocol{

    const MODE = 'mode';
    const POWER = 'power';
    const TEM = 'tem';
    const WIND = 'wind';
    const REGISTAR_BASE_ADDESS =0x07D0;
    const CONTROL_ONOFF_WIND_OFFSET = 0;
    const CONTROL_TEM_SET_OFFSET = 2;
    const CONTROL_MODE_SET_OFFSET = 1;
    const FUNC_CODE_QUERY = 0x04;
    const FUNC_CODE_CONTROL = 0x06;

    public static function address_query(&$data){
        $addr1 = intval(substr($data,0,1)) - 1;
        $addr2 = intval(substr($data,2,2));
        $addr = DaikinProtocol::REGISTAR_BASE_ADDESS + ($addr1*16+$addr2)*6;
        return chr($addr>>8) . chr($addr&0xFF);   
    }

    public static function address_control(&$cmd,&$data){
        $addr1 = intval(substr($data,0,1)) - 1;
        $addr2 = intval(substr($data,2,2));
        $addr = DaikinProtocol::REGISTAR_BASE_ADDESS + ($addr1*16+$addr2)*3;
        if($cmd == DaikinProtocol::POWER || $cmd == DaikinProtocol::WIND)
            $addr += DaikinProtocol::CONTROL_ONOFF_WIND_OFFSET;
        else if($cmd == DaikinProtocol::TEM)
            $addr += DaikinProtocol::CONTROL_TEM_SET_OFFSET;
        else if($cmd == DaikinProtocol::MODE)
            $addr += DaikinProtocol::CONTROL_MODE_SET_OFFSET;
        return chr($addr>>8) . chr($addr&0xFF);
    }

    public static function query($device,$data){
        $msg = chr(intval($device));
        $msg .= chr(DaikinProtocol::FUNC_CODE_QUERY);
        $msg .= DaikinProtocol::address_query($data);
        $msg .= chr(0x00);
        $msg .= chr(0x06);
        crc16_v2($msg);
        return $msg;
    }

    public static function control($device,$addr,$cmd,$data){
        $msg = chr(intval($device));
        $msg .= chr(DaikinProtocol::FUNC_CODE_CONTROL);
        $msg .= DaikinProtocol::address_control($cmd,$addr);
        if(strcmp($cmd,DaikinProtocol::MODE) == 0){
            $msg .= chr(0x00);
            $msg .= chr(intval($data));
        }else if(strcmp($cmd,DaikinProtocol::POWER) == 0){
            $msg .= chr(0xFF);
            if(intval($data) == 1)
                $msg .= chr(0x61);
            else
                $msg .= chr(0x60);
        }else if( strcmp($cmd,DaikinProtocol::WIND) == 0){
            $msg .= chr(intval($data));
            $msg .= chr(0xFF);
        }else{
            $msg .= chr(intval($data)*10/0x100);
            $msg .= chr(intval($data)*10%0x100);
        }
        crc16_v2($msg);
        return $msg;    
    }

    public static function isControlValueValid($cmd,$data){
        if(strcmp($cmd,DaikinProtocol::MODE) == 0)
            return $data !== null && ($data == 0 || preg_match('/^[1237]$/', $data));
        else if(strcmp($cmd,DaikinProtocol::POWER) == 0)
            return $data !== null && ($data == 0 || preg_match('/^1$/', $data));
        else if( strcmp($cmd,DaikinProtocol::WIND) == 0)
            return $data !== null && preg_match('/^(16|32|48|64|80)$/', $data);
        else
            return $data !== null && preg_match('/^-?\d{1,2}$/', $data);
    }    

    public static function jsonResponse($data){
        $code = ord(substr($data,1,1));
        if($code == DaikinProtocol::FUNC_CODE_QUERY){
            $wind='';
            $tem='';
            $mode='';
            $power='';
            $wind = ord(substr($data,3,1))&0xF0;
            $mode = ord(substr($data,6,1))&0x0F;
            $tem = ((float)(ord(substr($data,7,1))*0x100 + ord(substr($data,8,1))))/10;
            $power = ord(substr($data,4,1))&0x01;
            return array(
                'wind' => $wind,
                'tem' => $tem,
                'mode' => $mode,
                'power' => $power );
        }else if($code == DaikinProtocol::FUNC_CODE_CONTROL){
           
        }
        return array();
    }

}

?>
