#include "Ship.h"
#include "../db/DB.h"
#include "../crew_member/CrewMember.h"
#include <vector>
#include <algorithm>
#include "../globals.h"


Ship::Ship(int id, const std::string &name, float carry_capacity, Port *homeport, ShipPurpose *purpose,
           const std::vector<CrewMember *> &crew, float *location, const tm &overhaul_start_date) {
    this->id = id;
    this->name = name;
    this->carry_capacity = carry_capacity;
    this->homeport = homeport;
    this->purpose = purpose;
    this->crew = crew;
    this->location[0] = location[0];
    this->location[1] = location[1];
    this->overhaul_start_date = overhaul_start_date;
}

Cruise* Ship::get_current_cruise() {
    return nullptr;
}

std::vector<Ship> ShipMapper::get_all() {
    std::vector<Ship> out_vec;

    auto db = DB::get_instance();
    auto ret = SQLExecDirectA(db->stmt_handle,
                              (SQLCHAR*)"SELECT * FROM ships;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("ShipMapper::get_all (1)", SQL_HANDLE_STMT, db->stmt_handle);
    }
    int id;
    char* name = new char[128];
    float carry_capacity;
    Port* homeport; int homeport_id;
    ShipPurpose* purpose; int purpose_id;
    std::vector<CrewMember*> crew; int crew_member_id;
    float location[2];
    tm overhaul_start_date{}; SQL_DATE_STRUCT sql_overhaul_start_date{};

    SQLHSTMT stmt_handle_sub;
    SQLAllocStmt(db->conn_handle, &stmt_handle_sub);
    char* sub_stmt_str = new char[128];
    while (SQL_SUCCEEDED(SQLFetch(db->stmt_handle))) {
        SQLGetData(db->stmt_handle, 1,
                   SQL_INTEGER, &id, 1, nullptr);
        SQLGetData(db->stmt_handle, 2,
                   SQL_CHAR, name, 128, nullptr);
        SQLGetData(db->stmt_handle, 3,
                   SQL_C_FLOAT, &carry_capacity, 1, nullptr);
        SQLGetData(db->stmt_handle, 4,
                   SQL_INTEGER, &homeport_id, 1, nullptr);
        homeport = std::find_if(ports.begin(), ports.end(),
                                [=](Port& port){return port.id == homeport_id;}).operator->();
        SQLGetData(db->stmt_handle, 5,
                   SQL_INTEGER, &purpose_id, 1, nullptr);
        purpose = std::find_if(ship_purposes.begin(), ship_purposes.end(),
                               [=](ShipPurpose& purpose1){return purpose1.id == purpose_id;}).operator->();
        SQLGetData(db->stmt_handle, 6,
                   SQL_C_DOUBLE, &location, 2, nullptr);
        SQLGetData(db->stmt_handle, 7,
                   SQL_C_TYPE_DATE, &sql_overhaul_start_date, 1, nullptr);
        overhaul_start_date.tm_mday = sql_overhaul_start_date.day;
        overhaul_start_date.tm_mon = sql_overhaul_start_date.month - 1;
        overhaul_start_date.tm_year = sql_overhaul_start_date.year - 1900;

        sprintf(sub_stmt_str, "SELECT id FROM crew_members WHERE ship_id=%d ORDER BY last_name;", id);
        ret = SQLExecDirectA(stmt_handle_sub, (SQLCHAR*)sub_stmt_str, SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            DB::extract_error("ShipMapper::get_all (2)", SQL_HANDLE_STMT, stmt_handle_sub);
        }
        crew.clear();
        while (SQL_SUCCEEDED(SQLFetch(stmt_handle_sub))) {
            SQLGetData(stmt_handle_sub, 1,
                       SQL_INTEGER, &crew_member_id, 1, nullptr);
            crew.push_back(std::find_if(members.begin(), members.end(),
                                        [=](CrewMember& member){return member.id == crew_member_id;}).operator->());
        }
        SQLCloseCursor(stmt_handle_sub);

        out_vec.emplace_back(id, name, carry_capacity, homeport, purpose, crew, location, overhaul_start_date);
    }
    SQLCloseCursor(db->stmt_handle);

    return out_vec;
}

void ShipMapper::insert(const Ship& ship) {
    auto db = DB::get_instance();
    char* stmt_str = new char[1024];
    char* overhaul_start_date = new char[11];
    strftime(overhaul_start_date, 11, "%Y-%m-%d", &ship.overhaul_start_date);
    if (ship.id == 0) {
        sprintf(stmt_str, "INSERT INTO ships (name, carry_capacity, homeport_id, purpose_id, "
                          "location, overhaul_start_date) "
                          "VALUES ('%s', %.2f, %d, %d, '%.5f, %.5f', '%s');",
                          ship.name.c_str(), ship.carry_capacity, ship.homeport->id, ship.purpose->id,
                          ship.location[0], ship.location[1], overhaul_start_date);
    } else {
        sprintf(stmt_str, "INSERT INTO ships (id, name, carry_capacity, homeport_id, purpose_id, "
                          "overhaul_start_date) "
                          "VALUES (%d, '%s', %.2f, %d, %d, '%s') "
                          "ON CONFLICT (id) DO UPDATE SET "
                          "name=EXCLUDED.name, carry_capacity=EXCLUDED.carry_capacity, "
                          "homeport_id=EXCLUDED.homeport_id, purpose_id=EXCLUDED.purpose_id, "
                          "overhaul_start_date=EXCLUDED.overhaul_start_date;",
                          ship.id, ship.name.c_str(), ship.carry_capacity, ship.homeport->id, ship.purpose->id,
                          overhaul_start_date);
    }
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("ShipMapper::insert (1)", SQL_HANDLE_STMT, db->stmt_handle);
    }

    int ship_id;
    if (ship.id == 0) {
        sprintf(stmt_str, "SELECT id FROM ships ORDER BY id DESC LIMIT 1;");
        ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            DB::extract_error("ShipMapper::insert (2)", SQL_HANDLE_STMT, db->stmt_handle);
        }
        if (SQL_SUCCEEDED(SQLFetch(db->stmt_handle))) {
            SQLGetData(db->stmt_handle, 1,
                       SQL_INTEGER, &ship_id, 1, nullptr);
        }
        SQLCloseCursor(db->stmt_handle);
    } else {
        ship_id = ship.id;
    }
    sprintf(stmt_str, "UPDATE crew_members SET ship_id=null WHERE ship_id=%d;", ship_id);
    ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA) {
        DB::extract_error("ShipMapper::insert (3)", SQL_HANDLE_STMT, db->stmt_handle);
    }
    for (const auto &member: ship.crew) {
        sprintf(stmt_str, "UPDATE crew_members SET ship_id=%d WHERE id=%d;", ship_id, member->id);
        ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            DB::extract_error("ShipMapper::insert (4)", SQL_HANDLE_STMT, db->stmt_handle);
        }
    }
}

void ShipMapper::remove(const Ship& ship) {
    auto db = DB::get_instance();
    char* stmt_str = new char[1024];
    sprintf(stmt_str, "DELETE FROM ships WHERE id=%d;", ship.id);
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("ShipMapper::remove (2)", SQL_HANDLE_STMT, db->stmt_handle);
    }
}

