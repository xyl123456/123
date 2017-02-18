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
preg_match('/^[1-6]$/', $addr) || Response::generate(30000,'invalid params');
$value = $_GET['value'];
if($value  === null)
    Response::generate(30000,'invalid params');
($value == 0 || preg_match('/^1$/', $value) ) || Response::generate(30000,'invalid params');

$conn = new TCPConnection('127.0.0.1',1234);
if($conn->connect() < 0)
    Response::generate(30002,'connect error');
$send = Protocol::control($device,$addr,$value);
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
