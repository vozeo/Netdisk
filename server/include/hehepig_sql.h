#ifndef HEHEPIG_SQL___
#define HEHEPIG_SQL___

#include <mysql.h>
#include <vector>
#include <string>

typedef std::vector<std::vector<std::string>> hehepig_sqlres;

class hehepig_sql {
protected:
    std::string user, password, database;
    MYSQL *mysql;
    int status;

public:
    hehepig_sql(const char *user, const char *password, const char *database);
    ~hehepig_sql();

    void cmd(const char *command);
    hehepig_sqlres cmd_ret(const char* command);

    void try_connect();

    bool good() {return status==0;}
};

#endif