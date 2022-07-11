set names gbk;
DROP DATABASE IF EXISTS cloud;
CREATE DATABASE cloud;
USE cloud;


/*ufiles*/
CREATE TABLE ufiles (
    ufile_id    INT             NOT NULL    UNIQUE  AUTO_INCREMENT,

    parent_id   INT             NOT NULL,
    is_dir      INT             NOT NULL,

    name        CHAR(100)       NOT NULL,
    md5         CHAR(50)        NOT NULL    DEFAULT 'd41d8cd98f00b204e9800998ecf8427e',
    upload_time TIMESTAMP(0)    NOT NULL    DEFAULT CURRENT_TIMESTAMP(0)    ON UPDATE CURRENT_TIMESTAMP(0),

    PRIMARY KEY (parent_id, name)
) CHARSET=gbk;

INSERT INTO ufiles (ufile_id, parent_id, is_dir, name) VALUES (
    1, 1, 1, "root"
);


/*files*/
CREATE TABLE files (
    md5     CHAR(50)        NOT NULL    UNIQUE,
    size    BIGINT          NOT NULL,
    status  INT             NOT NULL    DEFAULT 0,
    slice_size  BIGINT      NOT NULL    DEFAULT 1048576,
    PRIMARY KEY (md5)
) CHARSET=gbk;
INSERT INTO files (md5, size, status) VALUES ("d41d8cd98f00b204e9800998ecf8427e", 0, 0);


/*slices*/
CREATE TABLE slices (
    md5     CHAR(50)        NOT NULL,
    id      BIGINT          NOT NULL,
    start   BIGINT          NOT NULL,
    size    BIGINT          NOT NULL    DEFAULT 0,
    status  INT             NOT NULL    DEFAULT 0,
    updtime TIMESTAMP       NOT NULL    DEFAULT CURRENT_TIMESTAMP(0) ON UPDATE CURRENT_TIMESTAMP(0),
    PRIMARY KEY (md5, id)
) CHARSET=gbk;

DELIMITER $$
DROP PROCEDURE IF EXISTS insertSlices;
CREATE PROCEDURE insertSlices(md5 CHAR(50), full_size BIGINT, slice_size BIGINT)
BEGIN
    DECLARE i, max_i, st BIGINT;
    SET max_i = (full_size+slice_size-1) DIV slice_size;
    SET i = 1;
    SET st = 0;
    WHILE i<max_i DO
        INSERT INTO slices (md5, id, start, size) VALUES (md5, i, st, slice_size);
        SET i = i+1;
        SET st = st + slice_size;
    END WHILE;
    INSERT INTO slices (md5, id, start, size) VALUES (md5, max_i, st, full_size-st);
END $$
DELIMITER ;


/*users*/
CREATE TABLE users (
    username    CHAR(50)        NOT NULL    UNIQUE,
    password    CHAR(50)        NOT NULL,
    rootdir_id  INT             NOT NULL,
    PRIMARY KEY (username)
) CHARSET=gbk;

INSERT INTO users (username, password, rootdir_id) VALUES (
    "root", MD5("root123"), 1
);