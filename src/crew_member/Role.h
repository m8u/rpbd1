#ifndef RPBD1_ROLE_H
#define RPBD1_ROLE_H

#include <windows.h>
#include <string>
#include <vector>


class Role {
public:
    int id;
    std::string title;

    Role(int id, std::string title);
};

class RoleMapper {
public:
    static std::vector<Role> get_all();
};


#endif //RPBD1_ROLE_H
