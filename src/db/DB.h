#ifndef RPBD1_DB_H
#define RPBD1_DB_H

#include <odbcinst.h>
#include <sql.h>
#include <sqlext.h>
#include <string>


class DB {
    DB();
    static void init();
public:
    SQLHENV env_handle;
    SQLHDBC conn_handle;
    SQLHSTMT stmt_handle;
    static DB* get_instance();
    static void extract_error(std::string fn, SQLSMALLINT type, SQLHANDLE handle);
};


#endif //RPBD1_DB_H
