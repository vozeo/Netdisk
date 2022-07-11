#ifndef HEHEPIG_UFILE___
#define HEHEPIG_UFILE___

#include <string>
#include "hehepig_sql.h"

// success: 0
// failed: -1

int ufile_maxid();
int ufile_get_parent_id(int dir);
int ufile_get_rootid(int dir);
bool ufile_is_dir(int dir);
int ufile_get_id_by_name(const std::string &name, int parent_id);
std::string ufile_auto_rename(const std::string &fname, int parent_id); // 根据父节点下其他文件名判重，自动加 (xxx) 后缀
std::string ufile_convert_name(const std::string &name); // 加上引号的转义

int ufile_dirname(std::string &ret, int curdir_id);

int ufile_ls(hehepig_sqlres &ret, int curdir_id);

int ufile_mkdir(int curdir_id, std::string dirname, int *new_id=NULL);

int ufile_touch(int curdir_id, std::string fname, std::string md5="d41d8cd98f00b204e9800998ecf8427e", int *new_id=NULL);


int ufile_issubdir(int dir1, int dir2);

int ufile_mv(int dir1, int dir2);


int ufile_rm(int dir);  //递归删除

int ufile_cp(int dir1, int dir2);   // 递归复制

int ufile_rename(int dir, const std::string &new_name);

int ufile_safe_rename(int dir, std::string new_name);


int ufile_get_md5(int dir, std::string &md5);

std::string ufile_get_detail(int dir);

std::string ufile_get_dfsdetail(int dir);

#endif