#include <cstdint>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath> 
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
ORAN::ORAN(){
    auto it = SCS2SlotsMap.find(Oran_SCS);
    if(it != SCS2SlotsMap.end())
    {
        slotsPerSubFrame = it->second;
    }
    else
    {
        std::cerr << "SCS not found in the SCS2SlotsMap" << std::endl;
    }
}

std::vector<uint8_t> ORAN::createORANPacket()
{
    std::vector<uint8_t> packet;
    packet.push_back(firstByte);
    packet.push_back(frameId);
    return packet;

}



/*
@prief: function to parse the configuration file
*/
void ORAN::parseConfigFile()
{
    // Open the configuration file
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Error opening file: " << configFilePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        // Skip comments
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos); // Remove comment
        }
        
        // Trim whitespace
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());

        // Parse the line
        if (line.find("Eth.LineRate") == 0) {
            LineRate = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Eth.CaptureSizeMs") == 0) {
            CaptureSizeMs = std::stoul(line.substr(line.find('=') + 1));
        }
        else if (line.find("Oran.SCS") == 0) {
            Oran_SCS = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Oran.MaxNrb") == 0) {
            Oran_MaxNrb = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Oran.NrbPerPacket") == 0) {
            Oran_NrbPerPacket = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Oran.PayloadType") == 0) {
            Oran_PayloadType = line.substr(line.find('=') + 1);
        } else if (line.find("Oran.Payload") == 0) {
            Oran_Payload = line.substr(line.find('=') + 1);
        }
 
    }

    configFile.close();
}

/*
@prief: function to print variables for debugging
*/
void ORAN::calceBytesPerPacket()
{
    BytesPerPacket = Oran_NrbPerPacket * BytesPerRb;
}


/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcTotalNumOfOranPackets()
{
    double packetsPerSymbol = std::ceil( static_cast<double>(Oran_MaxNrb) / static_cast<double>(Oran_NrbPerPacket));
    std::cout << "packetsPerSymbol: " << packetsPerSymbol << std::endl;
    uint64_t packetPerSecond = packetsPerSymbol * symbolPerSlot * slotsPerSubFrame * subFramePerFrame * framesPerSec;
    totalNumOfOranPackets = packetPerSecond * CaptureSizeMs / 1000;

}

/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcNumOfFrames()
{
    framesPerCapture = CaptureSizeMs / frameTimeMs;
}

void ORAN::printVariables()
{
    std::cout<<"\n";
    std::cout <<"******************************************************************************************\n";
    std::cout << "ORAN variables: " << "\n";
    std::cout << "LineRate: " << LineRate << "\n";
    std::cout << "CaptureSizeMs: " << CaptureSizeMs << "\n";
    std::cout << "MaxPacketSize: " << MaxPacketSize << "\n";
    std::cout << "Oran_SCS: " << Oran_SCS << "\n";
    std::cout << "Oran_MaxNrb: " << Oran_MaxNrb << "\n";
    std::cout << "Oran_NrbPerPacket: " << Oran_NrbPerPacket << "\n";
    std::cout << "Oran_PayloadType: " << Oran_PayloadType << "\n";
    std::cout << "Oran_Payload: " << Oran_Payload << "\n";
    std::cout << "slotsPerSubFrame: " << slotsPerSubFrame << "\n";
    std::cout << "BytesPerPacket: " << BytesPerPacket << "\n";
    std::cout << "totalNumOfOranPackets: " << totalNumOfOranPackets << "\n"; 
}
