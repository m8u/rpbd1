#include <algorithm>
#include "CrewMember.h"
#include "../db/DB.h"
#include <ctime>
#include "../globals.h"


Ship* CrewMember::get_ship() {
    auto ship = std::find_if(ships.begin(), ships.end(), [=](Ship ship) {
        return std::find_if(ship.crew.begin(),ship.crew.end(), [=](CrewMember* member) {
            return member->id == this->id;
        }) != ship.crew.end();
    });
    if (ship != ships.end())
        return ship.operator->();
    return nullptr;
}

CrewMember::CrewMember(int id, const std::string &first_name, const std::string &last_name, const std::string &patronymic,
                       const tm &birth_date, Role *role, int experience, int salary) {
    this->id = id;
    this->first_name = first_name;
    this->last_name = last_name;
    this->patronymic = patronymic;
    this->birth_date = birth_date;
    this->role = role;
    this->experience = experience;
    this->salary = salary;
}

std::vector<CrewMember> CrewMemberMapper::get_all() {
    std::vector<CrewMember> out_vec;

    auto db = DB::get_instance();
    auto ret = SQLExecDirectA(db->stmt_handle,
                              (SQLCHAR*)"SELECT id, first_name, last_name, patronymic, "
                                        "birth_date, role_id, experience, salary FROM crew_members ORDER BY last_name;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("CrewMemberMapper::get_all", SQL_HANDLE_STMT, db->stmt_handle);
    }
    int id;
    char* first_name = new char[64];
    char* last_name = new char[64];
    char* patronymic = new char[64];
    tm birth_date{}; SQL_DATE_STRUCT sql_birth_date{};
    Role* role; int role_id;
    int experience;
    int salary;
    while (SQL_SUCCEEDED(SQLFetch(db->stmt_handle))) {
        SQLGetData(db->stmt_handle, 1,
                   SQL_INTEGER, &id, 1, nullptr);
        SQLGetData(db->stmt_handle, 2,
                   SQL_CHAR, first_name, 64, nullptr);
        SQLGetData(db->stmt_handle, 3,
                   SQL_CHAR, last_name, 64, nullptr);
        SQLGetData(db->stmt_handle, 4,
                   SQL_CHAR, patronymic, 64, nullptr);
        SQLGetData(db->stmt_handle, 5,
                   SQL_C_TYPE_DATE, &sql_birth_date, 1, nullptr);
        birth_date.tm_mday = sql_birth_date.day;
        birth_date.tm_mon = sql_birth_date.month - 1;
        birth_date.tm_year = sql_birth_date.year - 1900;
        SQLGetData(db->stmt_handle, 6,
                   SQL_INTEGER, &role_id, 1, nullptr);
        role = std::find_if(roles.begin(), roles.end(),
                            [=](Role& role1){return role1.id == role_id;}).operator->();
        SQLGetData(db->stmt_handle, 7,
                   SQL_INTEGER, &experience, 1, nullptr);
        SQLGetData(db->stmt_handle, 8,
                   SQL_INTEGER, &salary, 1, nullptr);
        out_vec.emplace_back(id, first_name, last_name, patronymic, birth_date, role, experience, salary);
    }
    SQLCloseCursor(db->stmt_handle);

    return out_vec;
}

void CrewMemberMapper::insert(CrewMember member) {
    auto db = DB::get_instance();
    char* stmt_str_base_format = new char[1024];
    char* stmt_str_base = new char[1024];
    char* stmt_str = new char[1024];
    char* birth_date = new char[11];
    strftime(birth_date, 11, "%Y-%m-%d", &member.birth_date);
    if (member.id == 0) {
        strcpy(stmt_str_base_format,
               "INSERT INTO crew_members "
               "(%sfirst_name, last_name, patronymic, birth_date, role_id, experience, salary) "
                              "VALUES (%s%s);");
        if (member.get_ship() == nullptr) {
            sprintf(stmt_str_base, stmt_str_base_format,
                    "", "", "'%s', '%s', '%s', '%s', '%d', '%d', '%d'");
            sprintf(stmt_str, stmt_str_base,
                    member.first_name.c_str(), member.last_name.c_str(), member.patronymic.c_str(),
                    birth_date, member.role->id, member.experience, member.salary);
        } else {
            sprintf(stmt_str_base, stmt_str_base_format,
                    "ship_id, ", "'%d', ", "'%s', '%s', '%s', '%s', '%d', '%d', '%d'");
            sprintf(stmt_str, stmt_str_base,
                    member.get_ship()->id, member.first_name.c_str(), member.last_name.c_str(),
                    member.patronymic.c_str(), birth_date, member.role->id, member.experience, member.salary);
        }
    } else {
        strcpy(stmt_str_base_format,
               "INSERT INTO crew_members "
               "(%sid, first_name, last_name, patronymic, birth_date, role_id, experience, salary) "
               "VALUES (%s%s) "
               "ON CONFLICT (id) DO UPDATE SET first_name=EXCLUDED.first_name, last_name=EXCLUDED.last_name, patronymic=EXCLUDED.patronymic, "
               "birth_date=EXCLUDED.birth_date, role_id=EXCLUDED.role_id, experience=EXCLUDED.experience, salary=EXCLUDED.salary;");
        if (member.get_ship() == nullptr) {
            sprintf(stmt_str_base, stmt_str_base_format,
                    "", "", "'%d', '%s', '%s', '%s', '%s', '%d', '%d', '%d'");
            sprintf(stmt_str, stmt_str_base,
                    member.id, member.first_name.c_str(), member.last_name.c_str(), member.patronymic.c_str(),
                    birth_date, member.role->id, member.experience, member.salary);
        } else {
            sprintf(stmt_str_base, stmt_str_base_format,
                    "ship_id, ", "'%d', ", "'%d', '%s', '%s', '%s', '%s', '%d', '%d', '%d'");
            sprintf(stmt_str, stmt_str_base,
                    member.get_ship()->id, member.id, member.first_name.c_str(), member.last_name.c_str(),
                    member.patronymic.c_str(), birth_date, member.role->id, member.experience, member.salary);
        }
    }
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("CrewMemberMapper::insert", SQL_HANDLE_STMT, db->stmt_handle);
    }
}

void CrewMemberMapper::remove(const CrewMember& member) {
    auto db = DB::get_instance();
    char* stmt_str = new char[1024];
    sprintf(stmt_str, "DELETE FROM crew_members WHERE id=%d;", member.id);
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("CrewMemberMapper::remove", SQL_HANDLE_STMT, db->stmt_handle);
    }
}

