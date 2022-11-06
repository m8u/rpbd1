#include "Role.h"
#include "../db/DB.h"

Role::Role(int id, std::string title) {
    this->id = id;
    this->title = title;
}


std::vector<Role> RoleMapper::get_all() {
    std::vector<Role> out_vec;

    auto db = DB::get_instance();
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)"SELECT * FROM crew_roles ORDER BY title;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("RoleMapper::get_all", SQL_HANDLE_STMT, db->stmt_handle);
    }
    int id;
    char* title = new char[128];
    while (SQL_SUCCEEDED(SQLFetch(db->stmt_handle))) {
        SQLGetData(db->stmt_handle, 1, SQL_INTEGER, &id, 1, nullptr);
        SQLGetData(db->stmt_handle, 2, SQL_CHAR, title, 128, nullptr);
        out_vec.emplace_back(id, title);
    }
    SQLCloseCursor(db->stmt_handle);

    return out_vec;
}
