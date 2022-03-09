#ifndef MASS_MAP_H
#define MASS_MAP_H

#include "Navigator.h"

class MassMap
{
public:
    MassMap();
    ~MassMap();
    double FindMass(int Z, int A);
    std::string FindSymbol(int Z, int A);

private:
    std::unordered_map<std::string, double> massTable;
    std::unordered_map<int, std::string> elementTable;

    //constants
    static constexpr double u_to_mev = 931.4940954;
    static constexpr double electron_mass = 0.000548579909;
};

#endif
