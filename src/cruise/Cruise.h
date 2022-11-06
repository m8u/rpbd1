#ifndef RPBD1_CRUISE_H
#define RPBD1_CRUISE_H

#include "GeneralCargoPackageType.h"
#include "../charterer/Charterer.h"
#include "../port/Port.h"
#include <vector>


class Ship;
class PortEntry;

class Cruise {
public:
    int id;
    Ship* ship;
    GeneralCargoPackageType* general_cargo_package_type;
    Port* departure_port;
    Port* destination_port;
    std::vector<PortEntry*> port_entries;
    Charterer* charterer;

    Cruise(int id, Ship *ship, GeneralCargoPackageType *general_cargo_package_type, Port *departure_port,
           Port *destination_port, const std::vector<PortEntry*> &port_entries, Charterer *charterer);
};

class CruiseMapper {
public:
    static std::vector<Cruise> get_all();
    static void insert(Cruise cruise);
    static void remove(Cruise &cruise);
};


#endif //RPBD1_CRUISE_H
