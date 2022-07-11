#ifndef HEHEPIG_FILE___
#define HEHEPIG_FILE___

#define SLICE_SIZE  (1048576)
#define EXPIRE_SEC  (10)

#include <string>
#include "hehepig_sql.h"

// if exist return status
// else return -1
int file_check_md5(const std::string &md5);

// 0:   succeed
// -1:  failed
int file_init_file(const std::string &md5, long long size);

int file_init_slices(const std::string &md5, long long size);

int file_get_one_undone_slice(const std::string &md5, hehepig_sqlres &res);

int file_modify_slice(const std::string &md5, long long id, int new_status);

double file_get_progress(const std::string &md5);

std::string file_get_size(std::string md5);

long long file_get_slice_num(const std::string &md5);

#endif