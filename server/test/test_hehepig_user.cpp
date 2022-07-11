#include <cstdio>
#include <iostream>

#include "hehepig_user.h"

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
    int tmp, tmp1, tmp2;
    hehepig_sqlres res;

    for (;;) {
        cout <<"> ";
        fflush(stdout);
        cin >>op;
        if (op=="register") {
            cin >>str1 >>str2;
            cout <<user_register(str1, str2) <<endl;
        }
        else if (op=="login") {
            cin >>str1 >>str2;
            cout <<user_login(str1, str2) <<endl;
        }
        else if (op=="set") {
            cin >>tmp >>str1;
            cout <<user_set(tmp, str1) <<endl;
        }
        else if (op=="get") {
            cin >>tmp;
            str1 = "#";
            tmp = user_get(tmp, str1);
            cout <<tmp <<" " <<str1 <<endl;
        }
    }
}


int main()
{
    test_all();
    return 0;
}