<?php
include './socket.php';
require_once 'user.php';

session_start();

function checkPassword($pwd)
{
    $score = 0;
    $len = strlen($pwd);
    if ($len < 12 || $len > 45) {
        return true;
    }
    if (preg_match("/[0-9]+/", $pwd)) {
        $score++;
    }
    if (preg_match("/[a-z]+/", $pwd)) {
        $score++;
    }
    if (preg_match("/[A-Z]+/", $pwd)) {
        $score++;
    }
    if (preg_match("/[_|\-|+|=|*|!|@|#|$|%|^|&|(|)]+/", $pwd)) {
        $score++;
    }
    return $score < 3;
}

function checkUsername($name)
{
    $len = strlen($name);
    return $len < 5 || $len > 45 || (!preg_match("/^[0-9a-zA-Z]+$/", $name));
}

@$username = $_POST['username'];
@$password = $_POST['password'];


$password = md5($password);

$res = cppSocket(array("32", $username, $password));

if ($res->rootdir > 0) {
    $user = new user();
    $user->setRootdir($res->rootdir);
    $user->setName($username);
    $_SESSION['user'] = $user;
    http_response_code(200);
    echo json_encode(array('message' => "Sign in success!\nRedirecting to home...", 'rootdir' => $res->rootdir));
} else if ($res->rootdir == -1) {
    http_response_code(400);
    echo json_encode(array('message' => 'The username does not exist, please check the username and try again.'));
} else {
    http_response_code(400);
    echo json_encode(array('message' => 'The password is wrong, please check the password and try again.'));
}
