#include <algorithm>
#include <vector>
#include "Cruise.h"
#include "../ship/Ship.h"
#include "../db/DB.h"
#include "PortEntry.h"
#include "../globals.h"


Cruise::Cruise(int id, Ship *ship, GeneralCargoPackageType *general_cargo_package_type, Port *departure_port,
               Port *destination_port, const std::vector<PortEntry*> &port_entries, Charterer *charterer) {
    this->id = id;
    this->ship = ship;
    this->general_cargo_package_type = general_cargo_package_type;
    this->departure_port = departure_port;
    this->destination_port = destination_port;
    this->port_entries = port_entries;
    this->charterer = charterer;
}

std::vector<Cruise> CruiseMapper::get_all() {
    std::vector<Cruise> out_vec;
    auto db = DB::get_instance();
    auto ret = SQLExecDirectA(db->stmt_handle,
                              (SQLCHAR*)"SELECT * FROM cruises;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("CruiseMapper::get_all (1)", SQL_HANDLE_STMT, db->stmt_handle);
    }
    int id;
    Ship* ship; int ship_id;
    GeneralCargoPackageType* general_cargo; int general_cargo_id;
    Port* departure_port; int departure_port_id;
    Port* destination_port; int destination_port_id;
    std::vector<PortEntry*> new_port_entries; int port_entry_id;
    Charterer* charterer; int charterer_id;

    SQLHSTMT stmt_handle_sub;
    SQLAllocStmt(db->conn_handle, &stmt_handle_sub);
    char* sub_stmt_str = new char[128];
    while (SQL_SUCCEEDED(SQLFetch(db->stmt_handle))) {
        SQLGetData(db->stmt_handle, 1,
                   SQL_INTEGER, &id, 1, nullptr);
        SQLGetData(db->stmt_handle, 2,
                   SQL_INTEGER, &ship_id, 1, nullptr);
        ship = std::find_if(ships.begin(), ships.end(),
                            [=](Ship& found) {return found.id == ship_id;}).operator->();
        SQLGetData(db->stmt_handle, 3,
                   SQL_INTEGER, &general_cargo_id, 1, nullptr);
        general_cargo = std::find_if(
                general_cargo_package_types.begin(), general_cargo_package_types.end(),
                [=](GeneralCargoPackageType& found) {return found.id == general_cargo_id;}).operator->();
        SQLGetData(db->stmt_handle, 4,
                   SQL_INTEGER, &departure_port_id, 1, nullptr);
        departure_port = std::find_if(ports.begin(), ports.end(),
                            [=](Port& found) {return found.id == departure_port_id;}).operator->();
        SQLGetData(db->stmt_handle, 5,
                   SQL_INTEGER, &destination_port_id, 1, nullptr);
        destination_port = std::find_if(ports.begin(), ports.end(),
                                      [=](Port& found) {return found.id == destination_port_id;}).operator->();
        SQLGetData(db->stmt_handle, 6,
                   SQL_INTEGER, &charterer_id, 1, nullptr);
        charterer = std::find_if(charterers.begin(), charterers.end(),
                                        [=](Charterer& found) {return found.id == charterer_id;}).operator->();


        sprintf(sub_stmt_str, "SELECT id FROM ship_port_entries WHERE cruise_id=%d "
                              "ORDER BY destination_ts_planned;", id);
        ret = SQLExecDirectA(stmt_handle_sub, (SQLCHAR*)sub_stmt_str, SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            DB::extract_error("CruiseMapper::get_all (2)", SQL_HANDLE_STMT, stmt_handle_sub);
        }
        new_port_entries.clear();
        while (SQL_SUCCEEDED(SQLFetch(stmt_handle_sub))) {
            SQLGetData(stmt_handle_sub, 1,
                       SQL_INTEGER, &port_entry_id, 1, nullptr);
            auto entry = std::find_if(port_entries.begin(), port_entries.end(),
                                            [=](PortEntry& found){return found.id == port_entry_id;});
            if (entry != port_entries.end())
                new_port_entries.push_back(entry.operator->());
        }
        SQLCloseCursor(stmt_handle_sub);

        out_vec.emplace_back(id, ship, general_cargo,
                             departure_port, destination_port, new_port_entries, charterer);
    }
    SQLCloseCursor(db->stmt_handle);

    return out_vec;
}

void CruiseMapper::insert(Cruise cruise) {
    auto db = DB::get_instance();
    char* stmt_str = new char[1024];
    if (cruise.id == 0) {
        sprintf(stmt_str, "INSERT INTO cruises (ship_id, general_cargo_package_id, departure_port_id, "
                          "destination_port_id, charterer_id) VALUES (%d, %d, %d, %d, %d);",
                cruise.ship->id, cruise.general_cargo_package_type->id, cruise.departure_port->id,
                cruise.destination_port->id, cruise.charterer->id);
    } else {
        sprintf(stmt_str, "INSERT INTO cruises (id, ship_id, general_cargo_package_id, departure_port_id, "
                          "destination_port_id, charterer_id) VALUES (%d, %d, %d, %d, %d, %d) "
                          "ON CONFLICT (id) DO UPDATE SET ship_id=EXCLUDED.ship_id, "
                          "general_cargo_package_id=EXCLUDED.general_cargo_package_id, "
                          "departure_port_id=EXCLUDED.departure_port_id, destination_port_id=EXCLUDED.destination_port_id, "
                          "charterer_id=EXCLUDED.charterer_id;",
                cruise.id, cruise.ship->id, cruise.general_cargo_package_type->id, cruise.departure_port->id,
                cruise.destination_port->id, cruise.charterer->id);
    }
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("CruiseMapper::insert (1)", SQL_HANDLE_STMT, db->stmt_handle);
    }

    int cruise_id;
    if (cruise.id == 0) {
        sprintf(stmt_str, "SELECT id FROM cruises ORDER BY id DESC LIMIT 1;");
        ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            DB::extract_error("CruiseMapper::insert (2)", SQL_HANDLE_STMT, db->stmt_handle);
        }
        if (SQL_SUCCEEDED(SQLFetch(db->stmt_handle))) {
            SQLGetData(db->stmt_handle, 1,
                       SQL_INTEGER, &cruise_id, 1, nullptr);
        }
        SQLCloseCursor(db->stmt_handle);
    } else {
        cruise_id = cruise.id;
    }
    sprintf(stmt_str, "UPDATE ship_port_entries SET cruise_id=%d WHERE cruise_id is null;", cruise_id);
    ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA) {
        DB::extract_error("CruiseMapper::insert (3)", SQL_HANDLE_STMT, db->stmt_handle);
    }
}

void CruiseMapper::remove(Cruise &cruise) {
    auto db = DB::get_instance();
    char* stmt_str = new char[1024];
    sprintf(stmt_str, "DELETE FROM cruises WHERE id=%d;", cruise.id);
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("CruiseMapper::remove (2)", SQL_HANDLE_STMT, db->stmt_handle);
    }
}
