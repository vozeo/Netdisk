#include <cstdio>
#include <iostream>

#include "hehepig_file.h"

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


void test_all()
{
    string op, str1, str2;
    int tmp1, tmp2;
    hehepig_sqlres res;

    for (;;) {
        cout <<"> ";
        fflush(stdout);
        cin >>op;
        if (op=="check") {
            cin >>str1;
            cout <<file_check_md5(str1) <<endl;
        }
        else if (op=="init_file") {
            cin >>str1 >>tmp1;
            cout <<file_init_file(str1, tmp1) <<endl;
        }
        else if (op=="init_slice") {
            cin >>str1 >>tmp1;
            cout <<file_init_slices(str1, tmp1) <<endl;
        }
        else if (op=="get") {
            cin >>str1;
            cout <<file_get_one_undone_slice(str1, res) <<endl;
            show_res(res);
        }
        else if (op=="modify") {
            cin >>str1 >>tmp1 >>tmp2;
            cout <<file_modify_slice(str1, tmp1, tmp2) <<endl;
        }
        else if (op=="progress") {
            cin >>str1;
            cout <<file_get_progress(str1) <<endl;
        }
        else if (op=="size") {
            cin >>str1;
            cout <<file_get_size(str1) <<endl;
        }
    }
}


int main()
{
    test_all();
    return 0;
}