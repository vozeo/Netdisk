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


/* ��ʱ�ϵ�
// ��ȡ ufile_id ��ӵ���ߣ��浽 username ��
// 0: succeed
// -1: failed
int user_get(int ufile_id, std::string &username);

// ���� ufile_id ��ӵ����
// 0: succeeded
// -1: failed
int user_set(int ufile_id, std::string username);
*/

#endif