#include <string>
#include "hehepig_sql.h"
#include "hehepig_ufile.h"
#include "hehepig_user.h"
#include "hehepig_parameter.h"

//static const char *SQL_USER="root";
//static const char *SQL_PASSWD="root123";
//static const char *SQL_DATABASE="cloud";

//static hehepig_sql sql("root", "root123", "cloud");
static char buf[10086];

static int check_sql(hehepig_sql &sql)
{
    for (int i=0; i<3 && !sql.good(); i++) sql.try_connect();
    return sql.good();
}

/*
static hehepig_sqlres get_user_by_name(std::string username)
{
    hehepig_sqlres ret;
    if (!check_sql()) return ret;

    sprintf(buf, "SELECT username, password, rootdir_id FROM users WHERE username=\'%s\'", username.c_str());
    ret = sql.cmd_ret(buf);
    return ret;
}
*/

//===================================================================
//===================================================================

int user_register(std::string username, std::string password)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    hehepig_sqlres res;

    sprintf(buf, "SELECT COUNT(1) FROM users WHERE username=\'%s\'", username.c_str());
    res = sql.cmd_ret(buf);
    if (!sql.good() || !res.size() || res[0][0]!="0") return -1;

    int rootdir_id = ufile_maxid()+1;
    printf("%s %d\n", username.c_str(), rootdir_id);
    sprintf(buf, "INSERT INTO ufiles (ufile_id, parent_id, is_dir, name) VALUES (%d, %d, 1, \'%s\')", rootdir_id, rootdir_id, username.c_str());
    sql.cmd(buf);
    if (!sql.good()) return -3;

    sprintf(buf, "INSERT INTO users (username, password, rootdir_id) VALUES (\'%s\', \'%s\', %d)", username.c_str(), password.c_str(), rootdir_id);
    sql.cmd(buf);

    return rootdir_id;
}

int user_login(std::string username, std::string password)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    hehepig_sqlres res;

    sprintf(buf, "SELECT COUNT(1) FROM users WHERE username=\'%s\'", username.c_str());
    res = sql.cmd_ret(buf);
    if (!res.size() || res[0][0]=="0") return -1;

    sprintf(buf, "SELECT rootdir_id FROM users WHERE username=\'%s\' and password=\'%s\'", username.c_str(), password.c_str());
    res = sql.cmd_ret(buf);
    if (!res.size()) return -2;
    return atoi(res[0][0].c_str());
}


/*
int user_get(int ufile_id, std::string &username)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    hehepig_sqlres res;

    sprintf(buf, "SELECT username FROM ufile_user WHERE ufile_id=%d", ufile_id);
    res = sql.cmd_ret(buf);
    if (!res.size() || !res[0].size()) return -1;
    username = res[0][0];

    return 0;
}

int user_set(int ufile_id, std::string username)
{
    std::string tmpstr;
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    username = ufile_convert_name(username);

    if (user_get(ufile_id, tmpstr)!=0) {
        if (ufile_dirname(tmpstr, ufile_id)!=0) {
            puts(".");
            return -1;
        }
        sprintf(buf, "INSERT INTO ufile_user (ufile_id, username) VALUES (%d, \'%s\')", ufile_id, username.c_str());
        sql.cmd(buf);
        return sql.good() ? 0 : -1;
    }
    else if (tmpstr!=username) {
        sprintf(buf, "UPDATE ufile_user SET username=\'%s\' WHERE ufile_id=%d", username.c_str(), ufile_id);
        sql.cmd(buf);
        return sql.good() ? 0 : -1;
    }

    return 0;
}
*/