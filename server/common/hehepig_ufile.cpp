#include <string>
#include "hehepig_sql.h"
#include "hehepig_ufile.h"
#include "hehepig_file.h"
#include "hehepig_parameter.h"

//static const char *SQL_USER="root";
//static const char *SQL_PASSWD="root123";
//static const char *SQL_DATABASE="cloud";


//static hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
static char buf[10086];

static int check_sql(hehepig_sql &sql)
{
    for (int i=0; i<3 && !sql.good(); i++) sql.try_connect();
    return sql.good();
}

//========================================================================
//========================================================================

int ufile_maxid()
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return 0;
    hehepig_sqlres res = sql.cmd_ret("SELECT MAX(ufile_id) FROM ufiles");
    return (sql.good()&&res.size()) ? atoi(res[0][0].c_str()) : 0;
}

int ufile_get_parent_id(int dir)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;

    hehepig_sqlres tmpres;
    sprintf(buf, "SELECT parent_id FROM ufiles WHERE ufile_id=%d", dir);
    tmpres = sql.cmd_ret(buf);
    if (!sql.good() || !tmpres.size()) return -1;

    return atoi(tmpres[0][0].c_str());
}

int ufile_get_rootid(int dir)
{
    int tmp=-1;
    dir = ufile_get_parent_id(dir);
    while (tmp!=dir) {
        tmp = dir;
        dir = ufile_get_parent_id(dir);
    }
    return dir;
}

bool ufile_is_dir(int dir)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    sprintf(buf, "SELECT is_dir FROM ufiles WHERE ufile_id=%d", dir);
    hehepig_sqlres res = sql.cmd_ret(buf);
    return (sql.good() && res.size() && res[0][0]=="1");
}

int ufile_get_id_by_name(const std::string &name, int parent_id)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    sprintf(buf, "SELECT ufile_id FROM ufiles WHERE parent_id=%d and name=\'%s\'", parent_id, name.c_str());
    hehepig_sqlres res = sql.cmd_ret(buf);
    return (sql.good() && res.size()) ? atoi(res[0][0].c_str()) : -1;
}

std::string ufile_auto_rename(const std::string &fname, int parent_id)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    hehepig_sqlres res;
    int dotpos;
    std::string ret=fname;
    std::string name, ext;
    for (dotpos=fname.length()-1; dotpos>=0 && fname[dotpos]!='.'; dotpos--);
    if (dotpos<0) dotpos = fname.length();
    for (int i=0; i<dotpos; i++) name+=fname[i];
    for (int i=dotpos,maxi=fname.length(); i<maxi; i++) ext+=fname[i];
    int id=0;
    while (1) {
        sprintf(buf, "SELECT COUNT(1) FROM ufiles WHERE parent_id=%d and name=\'%s\'", parent_id, ret.c_str());
        res = sql.cmd_ret(buf);
        if (!sql.good() || !res.size() || res[0][0]=="0") {
            break;
        }
        ret = name+"("+std::to_string(++id)+")"+ext;
    }
    return ret;
}

std::string ufile_convert_name(const std::string &name)
{
    std::string ret;
    for (char ch: name) {
        if (ch=='\'' || ch=='\"') ret += '\\';
        ret += ch;
    }
    return ret;
}


int ufile_dirname(std::string &ret, int curdir_id)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    sprintf(buf, "SELECT name FROM ufiles WHERE ufile_id=%d", curdir_id);
    hehepig_sqlres res = sql.cmd_ret(buf);

    if (res.size()) {
        ret = res[0][0];
        return 0;
    }
    else {
        return -1;
    }
}

int ufile_ls(hehepig_sqlres &ret, int curdir_id)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    sprintf(buf, "SELECT  ufile_id, is_dir, name, upload_time, md5 FROM ufiles WHERE parent_id=%d and parent_id!=ufile_id ORDER BY is_dir DESC, name ASC", curdir_id);
    ret = sql.cmd_ret(buf);
    
    return sql.good() ? 0 : -1;
}

int ufile_mkdir(int curdir_id, std::string dirname, int *new_id)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    dirname = ufile_convert_name(dirname);
    dirname = ufile_auto_rename(dirname, curdir_id);
    //dirname = ufile_convert_name(dirname);
    sprintf(buf, "INSERT INTO ufiles (parent_id, is_dir, name) VALUES (%d, 1, \'%s\')", curdir_id, dirname.c_str());
    sql.cmd(buf);

    if (new_id!=NULL) {
        *new_id = ufile_get_id_by_name(dirname, curdir_id);
    }

    return sql.good() ? 0 : -1;
}

int ufile_touch(int curdir_id, std::string fname, std::string md5, int *new_id)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    fname = ufile_convert_name(fname);
    fname = ufile_auto_rename(fname, curdir_id);
    sprintf(buf, "INSERT INTO ufiles (parent_id, is_dir, name, md5) VALUES (%d, 0, \'%s\', \'%s\')", curdir_id, fname.c_str(), md5.c_str());
    sql.cmd(buf);

    if (new_id != NULL) {
        *new_id = ufile_get_id_by_name(fname, curdir_id);
    }

    return sql.good() ? 0 : -1;
}

int ufile_issubdir(int dir1, int dir2)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    if (dir1==dir2) return 1;
    hehepig_sqlres tmpres;
    int curdir, pdir;
    for (curdir=dir1; ; curdir=pdir) {
        pdir = ufile_get_parent_id(curdir);
        if (pdir<0) return 0;
        if (pdir==dir2) return 1;
        if (curdir==pdir) break;
    }
    return 0;
}

int ufile_mv(int dir1, int dir2)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    if (ufile_issubdir(dir2, dir1)) return -1;
    if (ufile_get_parent_id(dir1)==dir2) return 0;

    std::string name;

    sprintf(buf, "UPDATE ufiles SET parent_id=%d WHERE ufile_id=%d", 0, dir1);
    sql.cmd(buf);

    ufile_dirname(name, dir1);
    if (!check_sql(sql)) return -1;
    name = ufile_convert_name(name);
    name = ufile_auto_rename(name, dir2);
    ufile_rename(dir1, name);

    sprintf(buf, "UPDATE ufiles SET parent_id=%d WHERE ufile_id=%d", dir2, dir1);
    sql.cmd(buf);
    
    return sql.good() ? 0 : -1;
}

int ufile_rm(int dir)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    // ÏÈµÝ¹éºóÉ¾³ý
    hehepig_sqlres res;

    if (ufile_ls(res, dir) != 0) {
        return -1;
    }

    for (auto &row: res) {
        ufile_rm(atoi(row[0].c_str()));
    }

    sprintf(buf, "DELETE FROM ufiles WHERE ufile_id=%d\n", dir);
    sql.cmd(buf);
    printf("rm %d\n", dir);
    return 0;
}

int ufile_cp(int dir1, int dir2)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    // ÏÈ¸´ÖÆºóµÝ¹é
    hehepig_sqlres res;
    int new_dir1;

    if (ufile_issubdir(dir2, dir1)) {
        return -1;
    }

    // ¸´ÖÆdir1
    sprintf(buf, "SELECT is_dir, name, md5 FROM ufiles WHERE ufile_id=%d", dir1);
    res = sql.cmd_ret(buf);
    if (!res.size()) return -1;
    new_dir1 = ufile_maxid()+1;
    res[0][1]=ufile_convert_name(res[0][1]);
    res[0][1]=ufile_auto_rename(res[0][1], dir2);
    sprintf(buf, "INSERT INTO ufiles (ufile_id, parent_id, is_dir, name, md5) VALUES (%d, %d, %s, \'%s\', \'%s\')",
        new_dir1, dir2, res[0][0].c_str(), res[0][1].c_str(), res[0][2].c_str()
    );
    sql.cmd(buf);
    if (!sql.good()) return -1;
    printf("cp %d->%d\n", dir1, new_dir1);

    // µÝ¹é
    if (ufile_ls(res, dir1) != 0) return -1;
    for (auto &row: res) {
        ufile_cp(atoi(row[0].c_str()), new_dir1);
    }
    return 0;
}

int ufile_rename(int dir, const std::string &new_name)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    
    sprintf(buf, "UPDATE ufiles SET name=\'%s\' WHERE ufile_id=%d\n", new_name.c_str(), dir);
    sql.cmd(buf);

    return sql.good() ? 0 : -1;
}

int ufile_safe_rename(int dir, std::string new_name)
{
    int parent_id = ufile_get_parent_id(dir);
    if (parent_id < 0) return -1;

    new_name = ufile_auto_rename(new_name, parent_id);
    new_name = ufile_convert_name(new_name);
    return ufile_rename(dir, new_name);
}

int ufile_get_md5(int dir, std::string &md5)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;

    hehepig_sqlres res;

    sprintf(buf, "SELECT md5 FROM ufiles WHERE ufile_id=%d", dir);
    res = sql.cmd_ret(buf);

    if (!sql.good() || !res.size()) return -1;
    md5 = res[0][0];
    return 0;
}

std::string ufile_get_detail(int dir)
{    
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    std::string ret="{\"ret_code\":-1}";

    if (!check_sql(sql)) return ret;

    hehepig_sqlres res;
    std::string md5;
    long long slice_num;

    sprintf(buf, "SELECT name, is_dir FROM ufiles WHERE ufile_id=%d", dir);
    res = sql.cmd_ret(buf);
    if (!sql.good() || !res.size()) return ret;

    if (ufile_get_md5(dir, md5) != 0) return ret;
    slice_num = file_get_slice_num(md5);

    ret="{\"ret_code\":0";
    ret+=",\"name\":\""+res[0][0]+"\"";
    ret+=",\"is_dir\":"+res[0][1];
    if (!ufile_is_dir(dir)) {
        ret+=",\"md5\":\""+md5+"\"";
        sprintf(buf, ",\"slice_num\":%lld",slice_num);
        ret+=(std::string)(buf);
    }
    ret+="}";
    return ret;
}

std::string ufile_get_dfsdetail(int dir)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql) || dir<0) return "err";
    
    std::string ret;
    std::string md5="null";
    std::string name="null";
    ret="{";
    sprintf(buf, "\"ufile_id\":%d", dir);
    ret+=buf;
    ufile_dirname(name, dir);
    
    if (!ufile_is_dir(dir)) {
        ufile_get_md5(dir, md5);
        ret+=",\"md5\":\""+md5+"\"";
        ret+=",\"slice_num\":"+std::to_string(file_get_slice_num(md5));
    }
    ret+=",\"name\":\""+name+"\"";

    if (ufile_is_dir(dir)) {
        ret+=",\"subs\":[";
        
        hehepig_sqlres subs;
        ufile_ls(subs, dir);
        int flag_dot=0;
        for (auto &sub: subs) {
            if (flag_dot) ret+=",";
            int sub_id=atoi(sub[0].c_str());
            ret+=ufile_get_dfsdetail(sub_id);
            flag_dot=1;
        }
        ret+="]";
    }

    ret+="}";

    return ret;
}