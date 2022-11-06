#ifndef RPBD1_SHIPPURPOSE_H
#define RPBD1_SHIPPURPOSE_H

#include <windows.h>
#include <string>
#include <vector>


class ShipPurpose {
public:
    int id;
    std::string name;

    ShipPurpose(int id, std::string name);
};

class ShipPurposeMapper {
public:
    static std::vector<ShipPurpose> get_all();
};


#endif //RPBD1_SHIPPURPOSE_H
