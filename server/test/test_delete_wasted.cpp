#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include "hehepig_sql.h"

using namespace std;

void show_res(const hehepig_sqlres &res)
{
    for (auto &i: res) {
        for (auto &j: i) {
            cout <<j <<'\t';
        }
        cout <<endl;
    }
    cout <<endl;
}

void delete_wasted_md5()
{
    hehepig_sql sql("root", "root123", "cloud");
    show_res(sql.cmd_ret("SELECT DISTINCT md5 FROM files WHERE md5 NOT IN (SELECT DISTINCT md5 FROM ufiles)"));
    sql.cmd("DELETE FROM slices WHERE md5 NOT IN (SELECT DISTINCT md5 FROM ufiles)");
    sql.cmd("DELETE FROM files WHERE md5 NOT IN (SELECT DISTINCT md5 FROM ufiles)");
}


int main()
{
    delete_wasted_md5();
    return 0;
}