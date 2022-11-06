#ifndef RPBD1_PORTENTRY_H
#define RPBD1_PORTENTRY_H

#include "Cruise.h"
#include "../port/Port.h"
#include <string>
#include <ctime>


class PortEntry {
public:
    int id;
    Port* port;
    tm destination_ts_planned;
    tm* destination_ts_actual;
    tm departure_ts_planned;
    tm* departure_ts_actual;
    std::string destination_delay_reason;
    std::string departure_delay_reason;

    PortEntry(Port* port, tm destination_ts_planned, tm departure_ts_planned);
    PortEntry(int id, Port* port, tm destination_ts_planned, tm* destination_ts_actual, tm departure_ts_planned,
              tm* departure_ts_actual, std::string destination_delay_reason, std::string departure_delay_reason);
    Cruise* get_cruise();
};

class PortEntryMapper {
public:
    static std::vector<PortEntry> get_all();
    static void insert(const PortEntry&);
};

#endif //RPBD1_PORTENTRY_H
