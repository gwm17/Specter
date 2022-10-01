/*
    MassMap.h
    A represnetation of the AMDC mass table. We provide capability to retrieve the mass of an isotope
    as well as the isotopic symbol. This sort of code is pretty ubiquitous in flexible nuclear physics
    analysis.

    GWM -- Feb 2022
*/
#ifndef MASS_MAP_H
#define MASS_MAP_H

#include "Specter.h"

class MassMap
{
public:
    MassMap();
    ~MassMap();
    double FindMass(uint32_t Z, uint32_t A);
    std::string FindSymbol(uint32_t Z, uint32_t A);

private:
    //As demonstrated elsewhere, using Szudzik pairing function to make unique id for two unsigned ints
    uint32_t GenerateID(uint32_t Z, uint32_t A)
    {
			return Z >= A ? (Z * Z + Z + A) : (A * A + Z);
    }
    std::unordered_map<uint32_t, double> massTable;
    std::unordered_map<uint32_t, std::string> elementTable;

    //constants
    static constexpr double s_u2MeV = 931.4940954;
    static constexpr double s_eMass = 0.000548579909;
};

#endif
