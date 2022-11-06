#include <windows.h>
#include "DB.h"
#include <iostream>
#include <fstream>

DB* db;

void DB::init() {
    std::ifstream init_file("init.sql");
    if (!init_file.is_open()) {
        std::cout << "'init.sql' not found" << std::endl;
        exit(1);
    }
    std::string line;
    std::string init_statements;
    while (std::getline(init_file, line)) {
        init_statements.append(line);
    }
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)init_statements.c_str(), init_statements.length());
    if (SQL_SUCCEEDED(ret)) {
        std::cout << "Init successful!" << std::endl;
    } else {
        DB::extract_error("DB::init", SQL_HANDLE_STMT, db->stmt_handle);
    }
}

DB::DB() {
    SQLRETURN ret;
    SQLAllocEnv(&(this->env_handle));
    SQLSetEnvAttr(this->env_handle, SQL_ATTR_ODBC_VERSION, (void*)(SQL_OV_ODBC3), 0);
    SQLAllocConnect(this->env_handle, &(this->conn_handle));

    char *conn_str = new char[1024];
    sprintf(
            conn_str,
            "Driver={PostgreSQL Unicode(x64)};Server=%s;Port=%s;Database=%s;Uid=%s;Pwd=%s;",
            getenv("HOST"),
            getenv("PORT"),
            getenv("DATABASE"),
            getenv("USER"),
            getenv("PASSWORD")
    );

    ret = SQLDriverConnectA(
            this->conn_handle, nullptr,
            (SQLCHAR*)conn_str, SQL_NTS,
            nullptr, 0,
            nullptr, SQL_DRIVER_NOPROMPT
    );

    if (SQL_SUCCEEDED(ret)) {
        std::cout << "Database connection successful!" << std::endl;
    } else {
        DB::extract_error("DB::DB (constructor)", SQL_HANDLE_DBC, this->conn_handle);
        exit(1);
    }

    SQLAllocStmt(this->conn_handle, &(this->stmt_handle));
}

DB* DB::get_instance() {
    if (db == nullptr) {
        db = new DB;
        init();
    }
    return db;
}

void DB::extract_error(std::string fn, SQLSMALLINT type, SQLHANDLE handle) {
    SQLINTEGER   i = 0;
    SQLINTEGER   native;
    SQLWCHAR     state[7];
    SQLWCHAR     text[256];
    SQLSMALLINT  len;
    SQLRETURN    ret;

    fprintf(stderr, "\nError in function %s\n\n", fn.c_str());

    do
    {
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
                            sizeof(text), &len);
        if (SQL_SUCCEEDED(ret))
            fprintf(stderr, "%ls:%ld:%ld:%ls\n", state, i, native, text);
    } while(ret == SQL_SUCCESS);
}
