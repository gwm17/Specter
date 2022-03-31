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
    std::ifstream massfile("Assets/amdc2016_mass.txt");
    if (massfile.is_open())
    {
        std::string junk, A, element;
        int Z;
        double atomicMassBig, atomicMassSmall, isotopicMass;
        getline(massfile, junk);
        getline(massfile, junk);
        while (massfile >> junk)
        {
            massfile >> Z >> A >> element >> atomicMassBig >> atomicMassSmall;
            isotopicMass = (atomicMassBig + atomicMassSmall * 1e-6 - Z * electron_mass) * u_to_mev;
            std::string key = "(" + std::to_string(Z) + "," + A + ")";
            massTable[key] = isotopicMass;
            elementTable[Z] = element;
        }
    }
    else
    {
        NAV_ERROR("Unable to load mass file! Make sure that the Resources folder exists!");
    }
}

MassMap::~MassMap() {}

//Returns nuclear mass in MeV
double MassMap::FindMass(int Z, int A)
{
    std::string key = "(" + std::to_string(Z) + "," + std::to_string(A) + ")";
    auto data = massTable.find(key);
    if (data == massTable.end())
    {
        NAV_ERROR("Invalid nucleus at MassMap::FindMass()! Z: {0} A: {1}", Z, A);
        return 0.0;
    }
    return data->second;
}

//returns element symbol
std::string MassMap::FindSymbol(int Z, int A)
{
    auto data = elementTable.find(Z);
    if (data == elementTable.end())
    {
        NAV_ERROR("Invalid nucleus at MassMap::FindSymbol()! Z: {0} A: {1}", Z, A);
        return "";
    }
    std::string fullsymbol = std::to_string(A) + data->second;
    return fullsymbol;
}