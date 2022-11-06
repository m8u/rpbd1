#ifndef RPBD1_CREWMEMBER_H
#define RPBD1_CREWMEMBER_H

#include <windows.h>
#include <string>
#include <ctime>
#include "../ship/Ship.h"
#include "Role.h"
#include <vector>


class CrewMember {
public:
    int id;
    std::string first_name;
    std::string last_name;
    std::string patronymic;
    tm birth_date{};
    Role* role;
    int experience;
    int salary;

    CrewMember(int id, const std::string &firstName, const std::string &lastName, const std::string &patronymic,
               const tm &birthDate, Role *role, int experience, int salary);

    Ship* get_ship();
};

class CrewMemberMapper {
public:
    static std::vector<CrewMember> get_all();
    static void insert(CrewMember);
    static void remove(const CrewMember&);
};


#endif //RPBD1_CREWMEMBER_H
