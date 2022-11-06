#include <algorithm>
#include <vector>
#include "PortEntry.h"
#include "../db/DB.h"
#include "../globals.h"


PortEntry::PortEntry() = default;

PortEntry::PortEntry(Port *port, tm destination_ts_planned, tm departure_ts_planned) {
    this->id = 0;
    this->port = port;
    this->destination_ts_planned = destination_ts_planned;
    this->destination_ts_actual = nullptr;
    this->departure_ts_planned = departure_ts_planned;
    this->departure_ts_actual = nullptr;
    this->destination_delay_reason = "";
    this->departure_delay_reason = "";
}

PortEntry::PortEntry(int id, Port* port, tm destination_ts_planned, tm* destination_ts_actual, tm departure_ts_planned,
           tm* departure_ts_actual, std::string destination_delay_reason, std::string departure_delay_reason) {
    this->id = id;
    this->port = port;
    this->destination_ts_planned = destination_ts_planned;
    this->destination_ts_actual = destination_ts_actual;
    this->departure_ts_planned = departure_ts_planned;
    this->departure_ts_actual = departure_ts_actual;
    this->destination_delay_reason = destination_delay_reason;
    this->departure_delay_reason = departure_delay_reason;
}

std::vector<PortEntry> PortEntryMapper::get_all() {
    std::vector<PortEntry> out_vec;

    auto db = DB::get_instance();
    auto ret = SQLExecDirectA(db->stmt_handle,
                              (SQLCHAR*)"SELECT id, port_id, destination_ts_planned, destination_ts_actual, "
                                        "departure_ts_planned, departure_ts_actual, destination_delay_reason, "
                                        "departure_delay_reason FROM ship_port_entries "
                                        "ORDER BY destination_ts_planned;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("PortEntryMapper::get_all", SQL_HANDLE_STMT, db->stmt_handle);
    }
    int id;
    Port* port; int port_id;
    tm destination_ts_planned{}; SQL_TIMESTAMP_STRUCT sql_destination_ts_planned{};
    tm* destination_ts_actual; SQL_TIMESTAMP_STRUCT sql_destination_ts_actual{};
    tm departure_ts_planned{}; SQL_TIMESTAMP_STRUCT sql_departure_ts_planned{};
    tm* departure_ts_actual; SQL_TIMESTAMP_STRUCT sql_departure_ts_actual{};
    char* destination_delay_reason = new char[1024];
    char* departure_delay_reason = new char[1024];
    SQLINTEGER indicator;
    while (SQL_SUCCEEDED(SQLFetch(db->stmt_handle))) {
        destination_ts_actual = nullptr;
        departure_ts_actual = nullptr;
        SQLGetData(db->stmt_handle, 1,
                   SQL_INTEGER, &id, 1, nullptr);
        SQLGetData(db->stmt_handle, 2,
                   SQL_INTEGER, &port_id, 1, nullptr);
        port = std::find_if(ports.begin(), ports.end(),
                            [=](Port& found) {return found.id == port_id;}).operator->();
        SQLGetData(db->stmt_handle, 3,
                   SQL_C_TYPE_TIMESTAMP, &sql_destination_ts_planned, 1, nullptr);
        destination_ts_planned.tm_year = sql_destination_ts_planned.year - 1900;
        destination_ts_planned.tm_mon = sql_destination_ts_planned.month - 1;
        destination_ts_planned.tm_mday = sql_destination_ts_planned.day;
        destination_ts_planned.tm_hour = sql_destination_ts_planned.hour;
        destination_ts_planned.tm_min = sql_destination_ts_planned.minute;
        SQLGetData(db->stmt_handle, 4,
                   SQL_C_TYPE_TIMESTAMP, &sql_destination_ts_actual, 1, (SQLLEN*)&indicator);
        if (indicator != SQL_NULL_DATA) {
            destination_ts_actual = new tm;
            destination_ts_actual->tm_year = sql_destination_ts_actual.year - 1900;
            destination_ts_actual->tm_mon = sql_destination_ts_actual.month - 1;
            destination_ts_actual->tm_mday = sql_destination_ts_actual.day;
            destination_ts_actual->tm_hour = sql_destination_ts_actual.hour;
            destination_ts_actual->tm_min = sql_destination_ts_actual.minute;
        }
        SQLGetData(db->stmt_handle, 5,
                   SQL_C_TYPE_TIMESTAMP, &sql_departure_ts_planned, 1, nullptr);
        departure_ts_planned.tm_year = sql_departure_ts_planned.year - 1900;
        departure_ts_planned.tm_mon = sql_departure_ts_planned.month - 1;
        departure_ts_planned.tm_mday = sql_departure_ts_planned.day;
        departure_ts_planned.tm_hour = sql_departure_ts_planned.hour;
        departure_ts_planned.tm_min = sql_departure_ts_planned.minute;
        SQLGetData(db->stmt_handle, 6,
                   SQL_C_TYPE_TIMESTAMP, &sql_departure_ts_actual, 1, (SQLLEN*)&indicator);
        if (indicator != SQL_NULL_DATA) {
            departure_ts_actual = new tm;
            departure_ts_actual->tm_year = sql_departure_ts_actual.year - 1900;
            departure_ts_actual->tm_mon = sql_departure_ts_actual.month - 1;
            departure_ts_actual->tm_mday = sql_departure_ts_actual.day;
            departure_ts_actual->tm_hour = sql_departure_ts_actual.hour;
            departure_ts_actual->tm_min = sql_departure_ts_actual.minute;
        }
        SQLGetData(db->stmt_handle, 7,
                   SQL_CHAR, destination_delay_reason, 1024, nullptr);
        SQLGetData(db->stmt_handle, 8,
                   SQL_CHAR, departure_delay_reason, 1024, nullptr);
        out_vec.push_back(PortEntry(id, port, destination_ts_planned, destination_ts_actual,
                             departure_ts_planned, departure_ts_actual,
                             destination_delay_reason, departure_delay_reason));
    }
    SQLCloseCursor(db->stmt_handle);

    return out_vec;
}

void PortEntryMapper::insert(const PortEntry& port_entry) {
    auto db = DB::get_instance();
    char* stmt_str = new char[1024];
    char* destination_ts_planned = new char[17];
    strftime(destination_ts_planned, 17, "%Y-%m-%d %H:%M", &port_entry.destination_ts_planned);
    char* destination_ts_actual = new char[19];
    if (port_entry.destination_ts_actual == nullptr)
        sprintf(destination_ts_actual, "null");
    else
        strftime(destination_ts_actual, 19, "'%Y-%m-%d %H:%M'", port_entry.destination_ts_actual);
    char* departure_ts_planned = new char[17];
    strftime(departure_ts_planned, 17, "%Y-%m-%d %H:%M", &port_entry.departure_ts_planned);
    char* departure_ts_actual = new char[19];
    if (port_entry.departure_ts_actual == nullptr)
        sprintf(departure_ts_actual, "null");
    else
        strftime(departure_ts_actual, 19, "'%Y-%m-%d %H:%M'", port_entry.departure_ts_actual);
    if (port_entry.id == 0) {
        sprintf(stmt_str, "INSERT INTO ship_port_entries (port_id, destination_ts_planned, destination_ts_actual, "
                          "departure_ts_planned, departure_ts_actual, destination_delay_reason, departure_delay_reason) "
                          "VALUES (%d, '%s', %s, '%s', %s, '%s', '%s');",
                port_entry.port->id, destination_ts_planned, destination_ts_actual, departure_ts_planned,
                departure_ts_actual, port_entry.destination_delay_reason.c_str(), port_entry.departure_delay_reason.c_str());
    } else {
        sprintf(stmt_str, "INSERT INTO ship_port_entries (id, port_id, destination_ts_planned, "
                          "destination_ts_actual, departure_ts_planned, departure_ts_actual, destination_delay_reason, "
                          "departure_delay_reason) "
                          "VALUES (%d, %d, '%s', %s, '%s', %s, '%s', '%s') ON CONFLICT (id) DO UPDATE SET "
                          "port_id=EXCLUDED.port_id, destination_ts_planned=EXCLUDED.destination_ts_planned, "
                          "destination_ts_actual=EXCLUDED.destination_ts_actual, "
                          "departure_ts_planned=EXCLUDED.departure_ts_planned, "
                          "departure_ts_actual=EXCLUDED.departure_ts_actual, "
                          "destination_delay_reason=EXCLUDED.destination_delay_reason, "
                          "departure_delay_reason=EXCLUDED.departure_delay_reason;",
                port_entry.id, port_entry.port->id, destination_ts_planned, destination_ts_actual, departure_ts_planned,
                departure_ts_actual, port_entry.destination_delay_reason.c_str(), port_entry.departure_delay_reason.c_str());
    }
    auto ret = SQLExecDirectA(db->stmt_handle, (SQLCHAR*)stmt_str, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        DB::extract_error("PortEntryMapper::insert", SQL_HANDLE_STMT, db->stmt_handle);
    }
}
