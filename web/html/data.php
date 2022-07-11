<?php
include './socket.php';
require_once 'user.php';

session_start();
$file_path = "../upload/";

$user = new user();
$user->setRootdir(-1);
if (isset($_SESSION["user"])) {
    $user = $_SESSION['user'];
}
if ($user->getRootdir() > 0) {
    $op = $_POST['op'];
    $par0 = $_POST['par0'];

    switch ($op) {
        case 10:
        case 15:
        case 17:
        case 18:
            echo cppSocket(array($op, $par0, $user->name), false);
            break;
        case 13:
        case 14:
            $par1 = $_POST['par1'];
            echo cppSocket(array($op, $par0, $par1, $user->name), false);
            break;
        case 21:
            $par1 = $_POST['par1'];
            echo cppSocket(array($op, $par0, $par1), false);
            break;
        case 12:
            $par1 = $_POST['par1'];
            $par2 = $_POST['par2'];
            $par1 = iconv("UTF-8", "GBK//IGNORE", $par1);
            echo cppSocket(array($op, $par0, $par1, $par2, $user->name), false);
            break;
        case 11:
        case 16:
            $par1 = $_POST['par1'];
            $par1 = iconv("UTF-8", "GBK//IGNORE", $par1);
            if (preg_match("/\/|\:|\*|\?|\"|\<|\>|\\\|\|/", $par1)) {
                json_encode(array("ret_code" => -1));
            } else {
                echo cppSocket(array($op, $par0, $par1, $user->name), false);
            }
            break;
        case 22:
            $par1 = $_POST['par1'];
            move_uploaded_file($_FILES['file']['tmp_name'], $file_path . $_FILES["file"]["name"]);
            echo cppSocket(array($op, $par0, $par1), false);
            break;
        case 25:
            echo $_POST;
            $file = $file_path . $_POST['par0'] . '_' . $_POST['par1'];
            if (file_exists($file)) {
                while (ob_get_level()) {
                    ob_end_clean();
                }
                header('Content-Description: File Transfer');
                header('Content-Type: application/octet-stream');
                header('Content-Disposition: attachment; filename='.basename($file));
                header('Content-Transfer-Encoding: binary');
                header('Expires: 0');
                header('Cache-Control: must-revalidate');
                header('Pragma: public');
                header('Content-Length: ' . filesize($file));
                ob_clean();
                flush();
                readfile($file);
            }
            break;
    }
} else {
    json_encode(array());
}
