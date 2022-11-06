#ifndef RPBD1_GLOBALS_H
#define RPBD1_GLOBALS_H

#include "charterer/Charterer.h"
#include "cruise/GeneralCargoPackageType.h"
#include "cruise/Cruise.h"
#include "cruise/PortEntry.h"
#include "crew_member/CrewMember.h"
#include "crew_member/Role.h"
#include "port/Port.h"
#include "ship/Ship.h"
#include "ship/ShipPurpose.h"


inline std::vector<Charterer> charterers;
inline std::vector<GeneralCargoPackageType> general_cargo_package_types;
inline std::vector<Cruise> cruises;
inline std::vector<PortEntry> port_entries;
inline std::vector<CrewMember> members;
inline std::vector<Role> roles;
inline std::vector<Port> ports;
inline std::vector<Ship> ships;
inline std::vector<ShipPurpose> ship_purposes;

#endif //RPBD1_GLOBALS_H
