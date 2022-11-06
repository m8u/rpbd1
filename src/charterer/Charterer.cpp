#include <windows.h>
#include "Charterer.h"
#include "../db/DB.h"

Charterer::Charterer(int id, std::string name, std::string address, std::string phone_number, std::string fax,
                     std::string email, std::string bankdetails_bank_name, std::string bankdetails_city,
                     std::string bankdetails_TIN, std::string bankdetails_account_number) {
    this->id = id;
    this->name = name;
    this->address = address;
    this->phone_number = phone_number;
    this->fax = fax;
    this->email = email;
    this->bankdetails_bank_name = bankdetails_bank_name;
    this->bankdetails_city = bankdetails_city;
    this->bankdetails_TIN = bankdetails_TIN;
    this->bankdetails_account_number = bankdetails_account_number;
}

std::vector<Charterer> ChartererMapper::get_all() {
    std::vector<Charterer> out_vec;

    auto db = DB::get_instance();
    auto ret = SQLExecDirectA(db->stmt_handle,
                              (SQLCHAR*)"SELECT * FROM charterers;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("ChartererMapper::get_all", SQL_HANDLE_STMT, db->stmt_handle);
    }
    int id;
    char* name = new char[128];
    char* address = new char[1024];
    char* phone_number = new char[16];
    char* fax = new char[16];
    char* email = new char[128];
    char* bankdetails_bank_name = new char[128];
    char* bankdetails_city = new char[64];
    char* bankdetails_TIN = new char[128];
    char* bankdetails_account_number = new char[128];
    while (SQL_SUCCEEDED(SQLFetch(db->stmt_handle))) {
        SQLGetData(db->stmt_handle, 1,
                   SQL_INTEGER, &id, 1, nullptr);
        SQLGetData(db->stmt_handle, 2,
                   SQL_CHAR, name, 128, nullptr);
        SQLGetData(db->stmt_handle, 3,
                   SQL_CHAR, address, 1024, nullptr);
        SQLGetData(db->stmt_handle, 4,
                   SQL_CHAR, phone_number, 16, nullptr);
        SQLGetData(db->stmt_handle, 5,
                   SQL_CHAR, fax, 16, nullptr);
        SQLGetData(db->stmt_handle, 6,
                   SQL_CHAR, email, 128, nullptr);
        SQLGetData(db->stmt_handle, 7,
                   SQL_CHAR, bankdetails_bank_name, 128, nullptr);
        SQLGetData(db->stmt_handle, 8,
                   SQL_CHAR, bankdetails_city, 64, nullptr);
        SQLGetData(db->stmt_handle, 9,
                   SQL_CHAR, bankdetails_TIN, 128, nullptr);
        SQLGetData(db->stmt_handle, 10,
                   SQL_CHAR, bankdetails_account_number, 128, nullptr);
        out_vec.emplace_back(id, name, address, phone_number, fax, email,
                             bankdetails_bank_name, bankdetails_city, bankdetails_TIN, bankdetails_account_number);
    }
    SQLCloseCursor(db->stmt_handle);

    return out_vec;
}

void ChartererMapper::insert(const Charterer& charterer) {
    auto db = DB::get_instance();
    char* stmt_str_base = new char[1024];
    char* stmt_str = new char[1024];
    if (charterer.id == 0) {
        strcpy(stmt_str_base, "INSERT INTO charterers "
                              "(name, address, phonenumber, fax, email, "
                              "bankdetails_bank_name, bankdetails_city, \"bankdetails_TIN\", bankdetails_account_number) "
                              "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s');");
        sprintf(stmt_str, stmt_str_base,
                charterer.name.c_str(), charterer.address.c_str(), charterer.phone_number.c_str(),
                charterer.fax.c_str(), charterer.email.c_str(),
                charterer.bankdetails_bank_name.c_str(), charterer.bankdetails_city.c_str(),
                charterer.bankdetails_TIN.c_str(), charterer.bankdetails_account_number.c_str());
    } else {
        strcpy(stmt_str_base, "INSERT INTO charterers "
                              "(id, name, address, phonenumber, fax, email, "
                              "bankdetails_bank_name, bankdetails_city, \"bankdetails_TIN\", bankdetails_account_number) "
                              "VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s') "
                              "ON CONFLICT (id) DO UPDATE SET "
                              "name = EXCLUDED.name, address = EXCLUDED.address, phonenumber = EXCLUDED.phonenumber, "
                              "fax = EXCLUDED.fax, email = EXCLUDED.email, "
                              "bankdetails_bank_name = EXCLUDED.bankdetails_bank_name, "
                              "bankdetails_city = EXCLUDED.bankdetails_city, "
                              "\"bankdetails_TIN\" = EXCLUDED.\"bankdetails_TIN\", "
                              "bankdetails_account_number = EXCLUDED.bankdetails_account_number;");
        sprintf(stmt_str, stmt_str_base,
                charterer.id, charterer.name.c_str(), charterer.address.c_str(), charterer.phone_number.c_str(),
                charterer.fax.c_str(), charterer.email.c_str(),
                charterer.bankdetails_bank_name.c_str(), charterer.bankdetails_city.c_str(),
                charterer.bankdetails_TIN.c_str(), charterer.bankdetails_account_number.c_str());
    }
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("ChartererMapper::insert", SQL_HANDLE_STMT, db->stmt_handle);
    }
}

void ChartererMapper::remove(const Charterer& charterer) {
    auto db = DB::get_instance();
    char* stmt_str = new char[1024];
    sprintf(stmt_str, "DELETE FROM charterers WHERE id=%d;", charterer.id);
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("ChartererMapper::remove", SQL_HANDLE_STMT, db->stmt_handle);
    }
}