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

if (checkUsername($username)) {
    http_response_code(400);
    echo json_encode(array("message" => "The username should be between 5 and 45 characters and can only contain uppercase letters, lowercase letters and numbers."));
    exit;
}

if (checkPassword($password)) {
    http_response_code(400);
    echo json_encode(array("message" => "The password should be 12 to 45 characters and contain three of uppercase letters, lowercase letters, numbers and symbols."));
    exit;
}

$password = md5($password);

$res = cppSocket(array("31", $username, $password));

if ($res->rootdir > 0) {
    $user = new user();
    $user->setRootdir($res->rootdir);
    $user->setName($username);
    $_SESSION['user'] = $user;
    http_response_code(200);
    echo json_encode(array('message' => "Sign up success!\nRedirecting to home...", 'rootdir' => $res->rootdir));
} else {
    http_response_code(400);
    echo json_encode(array('message' => 'The username already exists, please change the username and try again.'));
}
