#ifndef RPBD1_GENERALCARGOPACKAGETYPE_H
#define RPBD1_GENERALCARGOPACKAGETYPE_H

#include <windows.h>
#include <string>
#include <vector>


class GeneralCargoPackageType {
public:
    int id;
    std::string name;

    GeneralCargoPackageType(int id, std::string name);
};

class GeneralCargoPackageTypeMapper {
public:
    static std::vector<GeneralCargoPackageType> get_all();
};

#endif //RPBD1_GENERALCARGOPACKAGETYPE_H
