#include <cstdio>
#include <iostream>

#include "hehepig_ufile.h"
#include "hehepig_parameter.h"

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


void test_dirname()
{
    std::string tmpstr;
    cout <<ufile_dirname(tmpstr, 1) <<"\t: ";
    cout <<tmpstr <<endl;

    cout <<ufile_dirname(tmpstr, 3) <<"\t: ";
    cout <<tmpstr <<endl;
}

void test_ls()
{
    hehepig_sqlres tmp;
    cout <<"#" <<ufile_ls(tmp, 1) <<"#" <<endl;
    show_res(tmp);
    
    cout <<"#" <<ufile_ls(tmp, 3) <<"#" <<endl;
    show_res(tmp);

    cout <<"#" <<ufile_ls(tmp, -1) <<"#" <<endl;
    show_res(tmp);
}

void test_mkdir()
{
    cout <<ufile_mkdir(1, "dir1") <<endl;
    cout <<ufile_mkdir(1, "dirfff") <<endl;
}

void test_touch()
{
    cout <<ufile_touch(1, "f1") <<endl;
    cout <<ufile_touch(3, "f1.c") <<endl;
}

void test_all()
{
    string op, tmpstr;
    hehepig_sqlres res;

    int curdir_id=1;
    string curdir_name;

    for (;;) {
        if (!cin.good()) {
            printf("err\n");
            break;
        }
        ufile_dirname(curdir_name, curdir_id);
        cout <<"[" <<curdir_name <<"]> ";
        fflush(stdout);
        cin >>op;

        if (op=="ls") {
            if (ufile_ls(res, curdir_id)) {
                cout <<"fail." <<endl;
                continue;
            }
            show_res(res);
        }
        else if (op=="mkdir") {
            int tmp;
            cin >>tmpstr;
            if (ufile_mkdir(curdir_id, tmpstr, &tmp)) {
                cout <<"fail." <<endl;
                continue;
            }
            cout <<tmp <<endl;
        }
        else if (op=="touch") {
            cin >>tmpstr;
            if (ufile_touch(curdir_id, tmpstr)) {
                cout <<"fail." <<endl;
                continue;
            }
        }
        else if (op=="cd") {
            cin >>curdir_id;
        }
        else if (op=="issubdir") {
            int d1, d2;
            cin >>d1 >>d2;
            cout <<ufile_issubdir(d1, d2) <<endl;
        }
        else if (op=="mv") {
            int d1, d2;
            cin >>d1 >>d2;
            cout <<ufile_mv(d1, d2) <<endl;
        }
        else if (op=="rm") {
            int d;
            cin >>d;
            cout <<ufile_rm(d) <<endl;
        }
        else if (op=="cp") {
            int d1, d2;
            cin >>d1 >>d2;
            cout <<ufile_cp(d1, d2) <<endl;
        }
        else if (op=="rename") {
            int d;
            cin >>d >>op;
            cout <<ufile_safe_rename(d, op) <<endl;
        }
        else if (op=="dfs") {
            int d;
            cin >>d;
            cout <<ufile_get_dfsdetail(d) <<endl;
        }
    }
}

int main(int argc, char **argv)
{
    //test_dirname();
    //test_ls();
    //test_mkdir();
    //test_touch();
    if (argc!=4) {
        printf("usage: %s sql_user sql_passwd sql_database\n", argv[0]);
        return -1;
    }
    SQL_USER = argv[1];
    SQL_PASSWD = argv[2];
    SQL_DATABASE = argv[3];
    test_all();
    return 0;
}