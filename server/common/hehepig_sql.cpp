#include "hehepig_sql.h"
#include <mysql.h>
#include <vector>
#include <string>
#include <cstdio>


hehepig_sql::hehepig_sql(const char *user_, const char *password_, const char *database_)
: user(user_), password(password_), database(database_) {
    status = -1;
    try_connect();
}

hehepig_sql::~hehepig_sql()
{
    if (mysql!=NULL)
        mysql_close(mysql);
}

void hehepig_sql::cmd(const char *command) {
    if (mysql_query(mysql, command)) {
        printf("mysql_query failed(%s)\n", mysql_error(mysql));
        status=11;
        return;
    }
}

std::vector<std::vector<std::string>> hehepig_sql::cmd_ret(const char* command) {
    std::vector<std::vector<std::string>> ret;
    MYSQL_RES *result=NULL;
    MYSQL_ROW row;
    int rows, fields;

    cmd(command);
    if (!good()) {
        return ret;
    }

    if ((result = mysql_store_result(mysql)) == NULL) {
        printf("mysql_store_result failed\n");
        status = 21;
        return ret;
    }

    rows = mysql_num_rows(result);
    fields = mysql_num_fields(result);

    ret.resize(rows);

    for (int i=0; i<rows; i++) {
        row = mysql_fetch_row(result);
        ret[i].resize(fields);
        for (int j=0; j<fields; j++) {
            ret[i][j] = row[j];
        }
    }

    mysql_free_result(result);

    return ret;
}

void hehepig_sql::try_connect()
{
    if (good()) return;

    status=0;

    if ((mysql = mysql_init(NULL)) == NULL) {
        printf("mysql_init failed\n");
        status=1;
        mysql=NULL;
    }
    
    if (!status && (mysql_real_connect(mysql, "localhost", user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0) == NULL)) {
        printf("mysql_real_connect failed(%s)\n", mysql_error(mysql));
        status=2;
        mysql_close(mysql);
        mysql=NULL;
    }
    
    mysql_set_character_set(mysql, "gbk");
}