#ifndef RPBD1_PORT_H
#define RPBD1_PORT_H

#include <windows.h>
#include <string>
#include <vector>

class Port {
public:
    int id;
    std::string name;

    Port(int id, std::string name);
};

class PortMapper {
public:
    static std::vector<Port> get_all();
};

#endif //RPBD1_PORT_H
