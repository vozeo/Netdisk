#include <string>
#include <stdlib.h>

#include "hehepig_file.h"

static const char *SQL_USER="root";
static const char *SQL_PASSWD="root123";
static const char *SQL_DATABASE="cloud";

//static hehepig_sql sql("root", "root123", "cloud");
static char buf[10086];

static int check_sql(hehepig_sql &sql)
{
    for (int i=0; i<3 && !sql.good(); i++) sql.try_connect();
    return sql.good();
}

//====================================================
//====================================================
int file_check_md5(const std::string &md5)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;
    hehepig_sqlres res;

    sprintf(buf, "SELECT status FROM files WHERE md5=\'%s\'", md5.c_str());
    res = sql.cmd_ret(buf);

    if (!sql.good() || !res.size()) return -1;
    return strtoll(res[0][0].c_str(), NULL, 10);
}


int file_init_file(const std::string &md5, long long size)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql) || size<0) return -1;
    
    long long slice_size = SLICE_SIZE;
    sprintf(buf, "INSERT INTO files (md5, size, status, slice_size) VALUES (\'%s\', %lld, 0, %lld)",
        md5.c_str(), size, slice_size
    );

    sql.cmd(buf);
    
    return sql.good() ? 0 : -1;
}

int file_init_slices(const std::string &md5, long long size)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql) || size<0) return -1;
    long long slice_size = SLICE_SIZE;
    sprintf(buf, "CALL insertSlices(\'%s\', %lld, %lld)", md5.c_str(), size, slice_size);
    sql.cmd(buf);
    return sql.good() ? 0 : -1;
}

int file_get_one_undone_slice(const std::string &md5, hehepig_sqlres &res)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;

    res.clear();
/*
    sprintf(buf, "SELECT COUNT(1) FROM slices WHERE md5=\'%s\' and status!=2", md5.c_str());
    res = sql.cmd_ret(buf);
    if (res.size() && res[0][0]=="0") return 0;
*/
    sprintf(buf, "SELECT id, start, size FROM slices WHERE md5=\'%s\' and ((status=0) or (status=1 and TIMESTAMPDIFF(SECOND, updtime, sysdate())>%d)) LIMIT 1",
        md5.c_str(), EXPIRE_SEC
    );

    res = sql.cmd_ret(buf);
    if (!sql.good()) return -1;
    if (!res.size()) {
        //sleep(1);
        return 0;
    }

    long long id = strtoll (res[0][0].c_str(), NULL, 10);
    file_modify_slice(md5, id, 0);
    file_modify_slice(md5, id, 1);

    return 0;
}

int file_modify_slice(const std::string &md5, long long id, int new_status)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return -1;

    sprintf(buf, "UPDATE slices SET status=%d WHERE md5=\'%s\' and id=%lld", new_status, md5.c_str(), id);
    sql.cmd(buf);

    return sql.good() ? 0 : -1;
}


double file_get_progress(const std::string &md5)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return 0;
    hehepig_sqlres res;

    double totcnt, curcnt;

    totcnt = file_get_slice_num(md5)+0.0001;
    
    sprintf(buf, "SELECT COUNT(1) FROM slices WHERE md5=\'%s\' and status=2", md5.c_str());
    res = sql.cmd_ret(buf);
    if (!sql.good() || !res.size()) return 0;
    curcnt=strtoll(res[0][0].c_str(), NULL, 10);
    
    return curcnt/totcnt;
}


std::string file_get_size(std::string md5)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return "0";
    hehepig_sqlres res;

    sprintf(buf, "SELECT size FROM files WHERE md5=\'%s\'", md5.c_str());
    res = sql.cmd_ret(buf);
    if (!sql.good() || !res.size() || !res[0].size()) return "0";

    return res[0][0];
}


long long file_get_slice_num(const std::string &md5)
{
    hehepig_sql sql(SQL_USER, SQL_PASSWD, SQL_DATABASE);
    if (!check_sql(sql)) return 0;
    hehepig_sqlres res;

    long long fullsize, slicesize;
    long long totnum;

    sprintf(buf, "SELECT size, slice_size FROM files WHERE md5=\'%s\'", md5.c_str());
    res = sql.cmd_ret(buf);
    if (!sql.good() || !res.size()) return 0;
    fullsize = strtoll(res[0][0].c_str(), NULL, 10);
    slicesize = strtoll(res[0][1].c_str(), NULL, 10);
    totnum = (fullsize+slicesize-1)/slicesize;

    return totnum;
}