<?php

#error_reporting(E_ALL ^ E_WARNING);
error_reporting(E_ALL ^ 0);
set_time_limit(0);

define("SUCCESS",     0);
define("ERROR_SOCKET_CREATE",     -1);
define("ERROR_SOCKET_CONNECT",     -2);
define("ERROR_SOCKET_READ_TIMEOUT",     -3);
define("ERROR_SOCKET_WRITE",     -4);
define("ERROR_SOCKET_WRITE_TIMEOUT",     -5);

class TCPConnection{    

    private $_port;
    
    private $_ipaddr;

    private $_socket;

    function __construct($ipaddr,$port){
        $this->_port = $port;
        $this->_ipaddr = $ipaddr;
    }

    public function connect(){
        $this->_socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        if($this->_socket < 0 ){
            //echo "socket_create() failed: reason: " . socket_strerror($socket) . "\n";
            return ERROR_SOCKET_CREATE;
        }
        socket_set_option($this->_socket,SOL_SOCKET,SO_RCVTIMEO,array("sec"=>5, "usec"=>0 ) );
        socket_set_option($this->_socket,SOL_SOCKET,SO_SNDTIMEO,array("sec"=>5, "usec"=>0 ) );
        $result = socket_connect($this->_socket, $this->_ipaddr, $this->_port);
        if (!$result) {
            //echo "socket_connect() failed.\nReason: ($result) " . socket_strerror($result) . "\n";
            return ERROR_SOCKET_CONNECT;
        }
        return $this->_socket;
    }

    public function disconnect(){
         if($this->_socket >= 0)
             socket_close($this->_socket);
    }

    function __destruct(){
        $this->disconnect();
    }
    
    public function read(){
        $out = socket_read($this->_socket, 128);//,PHP_BINARY_READ);
        //while($tmp = socket_read($this->_socket, 128)) {
        //    echo 'read = '.$tmp;
        //    $out .= $tmp;
        //}
        return $out;   
    }

    public function write($in){
        if(!socket_write($this->_socket, $in, strlen($in))) {
            //echo "socket_write() failed: reason: " . socket_strerror($this->_socket) . "\n";
            return ERROR_SOCKET_WRITE;
        }
        return SUCCESS; 
    }
  
}

?>
