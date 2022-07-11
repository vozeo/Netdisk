<?php
include '../Exception.php';
include '../Factory.php';
include '../Socket.php';

function cppSocket($input, $is_decode = true)
{
    $factory = new \Socket\Raw\Factory();
    $socket = $factory->createClient('localhost:20168');

    $send = "";
    foreach ($input as $key => $val) {
        $send = $send . $val;
        if ($key + 1 < count($input)) {
            $send = $send . "\t";
        } else {
            $send = $send . "\n";
        }
    }

    $socket->write($send);
    $res = $socket->read(100000);

    $socket->close();

    $content = iconv("GBK", "UTF-8//IGNORE", $res);
    if ($is_decode) {
        return json_decode($content);
    } else {
        return $content;
    }
}
