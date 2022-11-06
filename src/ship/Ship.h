#ifndef RPBD1_SHIP_H
#define RPBD1_SHIP_H

#include <windows.h>
#include "ShipPurpose.h"
#include "../port/Port.h"
#include "../cruise/Cruise.h"
#include <ctime>
#include <string>
#include <vector>

class CrewMember;

class Ship {
public:
    int id;
    std::string name;
    float carry_capacity;
    Port* homeport;
    ShipPurpose* purpose;
    std::vector<CrewMember*> crew;
    float location[2];
    tm overhaul_start_date;

    Ship(int id, const std::string &name, float carry_capacity, Port *homeport, ShipPurpose *purpose,
         const std::vector<CrewMember *> &crew, float *location, const tm &overhaul_start_date);

    Cruise *get_current_cruise();
};

class ShipMapper {
public:
    static std::vector<Ship> get_all();
    static void insert(const Ship&);
    static void remove(const Ship&);
};


#endif //RPBD1_SHIP_H
