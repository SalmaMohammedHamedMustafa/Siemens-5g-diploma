#include <iostream>
#include "ORAN.hpp"

/*
*SCS to number of slots map definition
*/
const std::map<uint32_t, uint16_t> ORAN::SCS2SlotsMap = 
{
    {15, 1},
    {30, 2},
    {60, 4},
    {120, 8},
    {240, 16},
    {480, 32},
    {640, 64}
};

/*
* @brief Constructor 
* it initializes the number of slots based on the SCS
* as it searches for the SCS in the SCS2SlotsMap
* if it finds it, it initializes the number of slots
* if it doesn't find it, it prints an error message
* @param SCS: Subcarrier spacing
*/
ORAN::ORAN(uint32_t SCS, uint16_t MaxNrb, uint16_t NrbPerPacket, uint32_t MaxORANPacketSize, uint16_t numOfFrames): 
SCS(SCS), MaxNrb(MaxNrb), NrbPerPacket(NrbPerPacket) , MaxORANPacketSize(MaxORANPacketSize), numOfFrames(numOfFrames)
{
    auto it = SCS2SlotsMap.find(SCS);
    if(it != SCS2SlotsMap.end())
    {
        numOfSlots = it->second;
    }
    else
    {
        std::cerr << "SCS not found in the SCS2SlotsMap" << std::endl;
    }
}

std::vector<uint8_t> ORAN::createORANPacket()
{
    std::vector<uint8_t> packet;
    packet.push_back(numOfSlots);
    printVariables();
    return packet;
}

/*
* @brief Print variables for debugging
*/
void ORAN::printVariables()
{
    std::cout<<"\n";
    std::cout <<"******************************************************************************************\n";
    std::cout << "ORAN variables: " << "\n";
    std::cout << "SCS: " << SCS << "\n";
    std::cout << "numOfSlots: " << numOfSlots << "\n";
    std::cout << "MaxNrb: " << MaxNrb << "\n";
    std::cout << "NrbPerPacket: " << NrbPerPacket << "\n";
    std::cout << "MaxORANPacketSize: " << MaxORANPacketSize << "\n";
}