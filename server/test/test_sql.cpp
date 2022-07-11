#include <cstdio>
#include <iostream>
#include <mysql.h>
#include <string>

using namespace std;


// ���� 0 �����ý������ݿ����
// ���� 1 ������Ҫ�������ݿ����������Ҫ���
// ���� 2 ������Ҫ�������ݿ����������Ҫ���
int get_str_sql(int &curdir_id, char *sql, MYSQL *mysql, string &curdir_name)
{
    MYSQL_RES   *result=NULL;
    MYSQL_ROW   row;
    string op, f;

    cin >>op;

    if (op == "pwd") {
        sprintf(sql, "SELECT name FROM ufiles WHERE ufile_id=%d", curdir_id);
        return 2;
    }
    else if (op == "ls") {
        sprintf(sql, "SELECT ufile_id, is_dir, name, upload_time FROM ufiles WHERE parent_id = %d ORDER BY is_dir DESC, name ASC", curdir_id);
        return 2;
    }
    else if (op == "touch") {
        cin >>f;
        sprintf(sql, "INSERT INTO ufiles (parent_id, is_dir, name) VALUES (%d, 0, \'%s\')", curdir_id, f.c_str());
        return 1;
    }
    else if (op == "mkdir") {
        cin >>f;
        sprintf(sql, "INSERT INTO ufiles (parent_id, is_dir, name) VALUES (%d, 1, \'%s\')", curdir_id, f.c_str());
        return 1;
    }
    else if (op == "cd") {
        cin >>f;
        if (f=="..") {
            sprintf(sql, "SELECT parent_id, name FROM ufiles WHERE ufile_id=%d", curdir_id);
        }
        else {
            sprintf(sql, "SELECT ufile_id, name FROM ufiles WHERE parent_id=%d and is_dir=1 and name=\'%s\'", curdir_id, f.c_str());
        }
        printf("%s\n", sql);
        // ��ѯ���ɹ�����0�����򷵻ط�0ֵ
        if (mysql_query(mysql, sql)) {
            printf("mysql_query failed(%s)\n", mysql_error(mysql));
            return -1;
        }

        // �洢��ѯ����������򷵻�NULL
        if ((result = mysql_store_result(mysql)) == NULL) {
            printf("mysql_store_result failed\n");
            return -1;
        }

        // ѭ����ȡ�������������ļ�¼
        while ((row = mysql_fetch_row(result)) != NULL) {
            curdir_id = atoi(row[0]);
            curdir_name = row[1];
        }
        return 0;
    }
    else if (op == "rm") {
        cin >>f;
    }
    sql[0]=0;

    return 0;
}


int test_ufile()
{
    MYSQL       *mysql;
    MYSQL_RES   *result=NULL;
    MYSQL_ROW   row;

    // ��ʼ�� mysql ������ʧ�ܷ��� NULL
    if ((mysql = mysql_init(NULL)) == NULL) {
        printf("mysql_init failed\n");
        return -1;
    }

    // �������ݿ⣬ʧ�ܷ��� NULL
    if (mysql_real_connect(mysql, "localhost", "root", "root123", "zzk", 0, NULL, 0) == NULL) {
        printf("mysql_real_connect failed(%s)\n", mysql_error(mysql));
        return -1;
    }

    // �����ַ���
    mysql_set_character_set(mysql, "gbk");

    int curdir_id=1;
    string curdir_name="/";
    for (int tmp; ; ) {
        printf("\n[%s]> ", curdir_name.c_str());
        fflush(stdout);
        char buf[1000];
        tmp = get_str_sql(curdir_id, buf, mysql, curdir_name); 

        if (tmp == 0) { continue; }

        printf("%s\n", buf);

        // ��ѯ���ɹ�����0�����򷵻ط�0ֵ
        if (mysql_query(mysql, buf)) {
            printf("mysql_query failed(%s)\n", mysql_error(mysql));
            return -1;
        }

        if (tmp == 1) { continue; }

        // �洢��ѯ����������򷵻�NULL
        if ((result = mysql_store_result(mysql)) == NULL) {
            printf("mysql_store_result failed\n");
            return -1;
        }

        // ��ӡ��ǰ��ѯ����¼������
        printf("select return %d records\n", (int)mysql_num_rows(result));
        int col = (int)mysql_num_fields(result);

        // ѭ����ȡ�������������ļ�¼
        while ((row = mysql_fetch_row(result)) != NULL) {
            for (int i=0; i<col; i++)
                printf("%s%c", row[i], "\t\n"[i==col-1]);
        }

        // �ͷ� result
        mysql_free_result(result);
    }



    // ��ѯ���ɹ�����0�����򷵻ط�0ֵ
    if (mysql_query(mysql, "SELECT ufile_id, is_dir, name, upload_time FROM ufiles WHERE parent_id = 1")) {
        printf("mysql_query failed(%s)\n", mysql_error(mysql));
        return -1;
    }

    // �洢��ѯ����������򷵻�NULL
    if ((result = mysql_store_result(mysql)) == NULL) {
        printf("mysql_store_result failed\n");
        return -1;
    }

    // ��ӡ��ǰ��ѯ����¼������
    printf("select return %d records\n", (int)mysql_num_rows(result));

    // ѭ����ȡ�������������ļ�¼
    while ((row = mysql_fetch_row(result)) != NULL) {
        printf("%s\t%s\t%s\t%s\n", row[0], row[1], row[2], row[3]);
    }

    // �ͷ� result
    mysql_free_result(result);

    // �ر�����
    mysql_close(mysql);

    return 0;
}

int main()
{
    return test_ufile();
}