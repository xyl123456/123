<?php

include '../foundation/response.php';
include '../foundation/data_transmit.php';
include '../foundation/http_code.php';
include 'protocol_transmit.php';

if($_SERVER['REQUEST_METHOD'] != "GET"){
    Response::generate(405,'method not allowed');
}

$device = empty($_GET['device']) ? '' : $_GET['device'];
preg_match('/^([1-9]|[1-5]\d|6[0-4])$/', $device) || Response::generate(30000,'invalid params');
$addr = empty($_GET['address']) ? '' : $_GET['address'];
preg_match('/^[1-9]\-(0[0-9]|1[0-5])$/', $addr) || Response::generate(30000,'invalid params');
$cmd = empty($_GET['cmd']) ? '' : $_GET['cmd'];
preg_match('/^(tem|wind|mode|power)$/', $cmd) || Response::generate(30000,'invalid params');
$value = $_GET['value'];
if(!Protocol::isControlValueValid($cmd,$value))
    Response::generate(30000,'invalid params');

$conn = new TCPConnection('127.0.0.1',1234);
if($conn->connect() < 0)
    Response::generate(30002,'connect error');
$send = Protocol::control($device,$addr,$cmd,$value);
if($conn->write($send) < 0)
    Response::generate(30003,'write error');
$out = $conn->read();
if(strlen($out) <= 0)
    Response::generate(30004,'read error');
if(!isset($out) || empty($out))
    Response::generate(30001,'timeout');
else{
    if(strcmp($out,$send) == 0)
        Response::generate(0,'success');
    else
        Response::generate(30005,'failed to control');
}
    
?>
