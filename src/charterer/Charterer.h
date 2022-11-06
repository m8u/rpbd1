#ifndef RPBD1_CHARTERER_H
#define RPBD1_CHARTERER_H

#include <string>
#include <vector>


class Charterer {
public:
    int id;
    std::string name;
    std::string address;
    std::string phone_number;
    std::string fax;
    std::string email;
    std::string bankdetails_bank_name;
    std::string bankdetails_city;
    std::string bankdetails_TIN;
    std::string bankdetails_account_number;

    Charterer(int id, std::string name, std::string address, std::string phone_number, std::string fax, std::string email, std::string bankdetails_bank_name,
              std::string bankdetails_city, std::string bankdetails_TIN, std::string bankdetails_account_number);
};

class ChartererMapper {
public:
    static std::vector<Charterer> get_all();
    static void insert(const Charterer&);
    static void remove(const Charterer&);
};


#endif //RPBD1_CHARTERER_H
