#include <iostream>
#include "ECPRI.hpp"
#include "../ORAN/ORAN.hpp"


/*
* @brief Constructor
* it initializes the payload size based on the MaxECPRIPacketSize
* @param SCS: Subcarrier spacing
*/   
ECPRI::ECPRI(uint32_t SCS, uint16_t MaxNrb, uint16_t NrbPerPacket, uint32_t MaxECPRIPacketSize, uint32_t CaptureTime): 
SCS(SCS), MaxNrb(MaxNrb), NrbPerPacket(NrbPerPacket), MaxECPRIPacketSize(MaxECPRIPacketSize), CaptureTime(CaptureTime)


{
    MaxORANPacketSize = MaxECPRIPacketSize - ECPRIPacketHeaderBytes;
    ORAN oran(MaxORANPacketSize);
    std::vector<uint8_t> oranPacket = oran.createORANPacket();

}


std::vector<uint8_t> ECPRI::createECPRIPacket()
{
    std::vector<uint8_t> packet;
    packet.push_back(firstByte);
    packet.push_back(message);
    packet.push_back(PC_RTC);
    printVariables();
    return packet;
}


/*
* @brief Print variables for debugging
*/
void ECPRI::printVariables()
{
    std::cout<<"\n";
    std::cout <<"******************************************************************************************\n";
    std::cout << "ECPRI variables: " << "\n";
    std::cout << "SCS: " << SCS << "\n";
    std::cout << "MaxNrb: " << MaxNrb << "\n";
    std::cout << "NrbPerPacket: " << NrbPerPacket << "\n";
    std::cout << "MaxECPRIPacketSize: " << MaxECPRIPacketSize << "\n";
}