<?php
class user {
    var $rootdir, $name;
    function setRootdir($rootdir) {
        $this->rootdir = $rootdir;
    }
    function getRootdir() {
        return $this->rootdir;
    }
    function setName($name) {
        $this->name = $name;
    }
    function getName() {
        return $this->name;
    }
}