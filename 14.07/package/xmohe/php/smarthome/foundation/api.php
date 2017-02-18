<?php 

abstract class Api{

    const JSON = 'json';

    const XML = 'xml';

    public static function factory($type = self::JSON){
        $type = isset($_GET['format']) ? $_GET['format'] : $type;
        $resultClass = ucwords($type);
        require_once('./responce/' . $type . '.php');
        return new $resultClass();
    }

    abstract function response($code, $message, $data);

}

Api::factory()->response(0,'success');

?>
