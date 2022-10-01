/*
    MassMap.h
    A represnetation of the AMDC mass table. We provide capability to retrieve the mass of an isotope
    as well as the isotopic symbol. This sort of code is pretty ubiquitous in flexible nuclear physics
    analysis.

    GWM -- Feb 2022
*/
#include "MassMap.h"

/*
    Read in AMDC mass file, preformated to remove excess info. Here assumes that by default
    the file is in a local directory Resources. Navigator build process handles this.
*/
MassMap::MassMap()
{
    SPEC_PROFILE_FUNCTION();
    std::ifstream massfile("Assets/amdc2016_mass.txt");
    if (massfile.is_open())
    {
        std::string junk, element;
        uint32_t Z, A, key;
        double atomicMassBig, atomicMassSmall, isotopicMass;
        getline(massfile, junk);
        getline(massfile, junk);
        while (massfile >> junk)
        {
            massfile >> Z >> A >> element >> atomicMassBig >> atomicMassSmall;
            isotopicMass = (atomicMassBig + atomicMassSmall * 1e-6 - Z * s_eMass) * s_u2MeV;
            key = GenerateID(Z, A);
            massTable[key] = isotopicMass;
            elementTable[key] = std::to_string(A) + element;
        }
    }
    else
    {
        SPEC_ERROR("Unable to load mass file! Make sure that the Resources folder exists!");
    }
}

MassMap::~MassMap() {}

//Returns nuclear mass in MeV
double MassMap::FindMass(uint32_t Z, uint32_t A)
{
    SPEC_PROFILE_FUNCTION();
    uint32_t key = GenerateID(Z, A);
    auto data = massTable.find(key);
    if (data == massTable.end())
    {
        SPEC_ERROR("Invalid nucleus at MassMap::FindMass()! Z: {0} A: {1}", Z, A);
        return 0.0;
    }
    return data->second;
}

//returns element symbol
std::string MassMap::FindSymbol(uint32_t Z, uint32_t A)
{
    SPEC_PROFILE_FUNCTION();
    static std::string nullResult = "";

    uint32_t key = GenerateID(Z, A);
    auto data = elementTable.find(key);
    if (data == elementTable.end())
    {
        SPEC_ERROR("Invalid nucleus at MassMap::FindSymbol()! Z: {0} A: {1}", Z, A);
        return nullResult;
    }
    return data->second;
}