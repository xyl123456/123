<?php

#include 'foundation/crc16.php';

require_once('foundation/crc16.php');

class RelayProtocol{

    const FUNC_CODE_QUERY = 0x04;
    const FUNC_CODE_CONTROL = 0x06;

    public static function query($device,$data){
        $msg = chr(intval($device));
        $msg .= chr(RelayProtocol::FUNC_CODE_QUERY);
        $msg .= chr(0x00) . chr(0x00) . chr(0x00);
        $msg .= chr(intval($data));
        crc16_v2($msg);
        return $msg;
    }

    public static function control($device,$addr,$data){
        $msg = chr(intval($device));
        $msg .= chr(RelayProtocol::FUNC_CODE_CONTROL);
        $msg .= chr(0x00) . chr(0x00);
        $msg .= chr(intval($addr)) . chr($data);
        crc16_v2($msg);
        return $msg;    
    }

    public static function jsonResponse($data){
        $code = ord(substr($data,1,1));
        if($code == RelayProtocol::FUNC_CODE_QUERY){
            $status = ord(substr($data,5,1));
            return array(
                'power' => $status);
        }else if($code == RelayProtocol::FUNC_CODE_CONTROL){
           
        }
        return array();
    }

}

?>
