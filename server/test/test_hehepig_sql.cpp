#include <cstdio>
#include <vector>
#include <string>
#include <iostream>

#include "hehepig_sql.h"

using namespace std;

int main()
{
    char buf[10086];
    hehepig_sql sql("root", "root123", "cloud");
    hehepig_sqlres res;

    for (;;) {

        cout <<"good: " <<sql.good() <<endl;
        if (!sql.good()) {
            cout <<"try_connect..." <<endl;
            sql.try_connect();
            cout <<"good: " <<sql.good() <<endl;
        }

        cin.getline(buf, 10086, '\n');
        res = sql.cmd_ret(buf);
        for (auto &i: res) {
            for (auto &j: i) {
                cout <<j <<'\t';
            }
            cout <<endl;
        }
        cout <<endl;
    }
}