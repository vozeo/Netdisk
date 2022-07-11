#ifndef HEHEPIG_USER___
#define HEHEPIG_USER___

#include <string>
#include "hehepig_sql.h"

// 0: succeeded
int user_register(std::string username, std::string password);

// <0: failed
//  -1: no username
//  -2: wrong passwd
// >0: rootdir
int user_login(std::string username, std::string password);


/* 暂时废掉
// 获取 ufile_id 的拥有者，存到 username 中
// 0: succeed
// -1: failed
int user_get(int ufile_id, std::string &username);

// 设置 ufile_id 的拥有者
// 0: succeeded
// -1: failed
int user_set(int ufile_id, std::string username);
*/

#endif