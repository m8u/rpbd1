#include "GeneralCargoPackageType.h"
#include "../db/DB.h"


GeneralCargoPackageType::GeneralCargoPackageType(int id, std::string name) {
    this->id = id;
    this->name = name;
}

std::vector<GeneralCargoPackageType> GeneralCargoPackageTypeMapper::get_all() {
    std::vector<GeneralCargoPackageType> out_vec;

    auto db = DB::get_instance();
    auto ret = SQLExecDirectA(db->stmt_handle,
                              (SQLCHAR*)"SELECT * FROM general_cargo_types;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("GeneralCargoPackageTypeMapper::get_all", SQL_HANDLE_STMT, db->stmt_handle);
    }
    int id;
    char* name = new char[128];
    while (SQL_SUCCEEDED(SQLFetch(db->stmt_handle))) {
        SQLGetData(db->stmt_handle, 1,
                   SQL_INTEGER, &id, 1, nullptr);
        SQLGetData(db->stmt_handle, 2,
                   SQL_CHAR, name, 128, nullptr);
        out_vec.emplace_back(id, name);
    }
    SQLCloseCursor(db->stmt_handle);

    return out_vec;
}
