#include <cstdint>
#include <iostream>
#include <vector>
#include <iomanip>
#include "ECPRI.hpp"



/*
* @brief Constructor
* it initializes the payload size based on the MaxECPRIPacketSize
* @param SCS: Subcarrier spacing
*/   
ECPRI::ECPRI(uint64_t MaxECPRIPacketSize): 
MaxECPRIPacketSize(MaxECPRIPacketSize)
{
    MaxORANPacketSize = MaxECPRIPacketSize - ECPRIPacketHeaderBytes;
    ORAN oran(MaxORANPacketSize); // Create an ORAN instance
    totalNumOfPackets = oran.getTotalNumOfPackets();
    
}

ECPRI::ECPRI()
{
    ORAN oran(MaxORANPacketSize); // Create an ORAN instance
    totalNumOfPackets = oran.getTotalNumOfPackets();
}


std::vector<uint8_t> ECPRI::createECPRIPacket()
{
    std::vector<uint8_t> packet;
    std::vector<uint8_t> oranPacket = getOranPacket(oran); 
    payload = oranPacket.size();
    // Add the first byte with all zeros
    packet.push_back(firstByte);
    // Add the message type byte
    packet.push_back(message);
    // Add the payload size
    packet.push_back((payload >> 8) & 0xFF); // higher byte
    packet.push_back(payload & 0xFF); // lower byte
    // Add the PC_RTC
    packet.push_back((PC_RTC >> 8) & 0xFF); // higher byte
    packet.push_back(PC_RTC & 0xFF); // lower byte
    // Add the sequence ID
    packet.push_back((seqId >> 8) & 0xFF); // higher byte
    packet.push_back(seqId & 0xFF); // lower byte
    updateSeqId();
    // Add the ORAN packet
    packet.insert(packet.end(), oranPacket.begin(), oranPacket.end());

    //printPacket(packet);

    return packet;
}

void ECPRI::updateSeqId()
{
    seqId++;
    if (seqId == 256)
    {
        seqId = 0;
    }
}

std::vector<uint8_t> ECPRI::getOranPacket(ORAN &oran)
{
    return oran.createORANPacket();
}

void ECPRI::printPacket(const std::vector<uint8_t>& packet)
{
    const size_t bytesPerLine = 4;  // Number of bytes per line
    size_t lineNumber = 0;  // Start line numbering from 0

    std::cout << "ECPRI packet:" << std::endl;
    
    // Loop through the packet and print 4 bytes per line
    for (size_t i = 0; i < packet.size(); i += bytesPerLine) {
        // Print line number
        std::cout << "Line " << lineNumber++ << ": ";

        // Print the next 4 bytes in hexadecimal format
        for (size_t j = 0; j < bytesPerLine && (i + j) < packet.size(); ++j) {
            // Print each byte in two-digit hex format
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(packet[i + j]) << " ";
        }

        std::cout << std::dec << std::endl;  // Switch back to decimal for line numbering
    }
}

/*
* @brief Print variables for debugging
*/
void ECPRI::printVariables()
{
    std::cout<<"\n";
    std::cout <<"******************************************************************************************\n";
    std::cout << "ECPRI variables: " << "\n";
    std::cout << "MaxECPRIPacketSize: " << MaxECPRIPacketSize << "\n";
}




/******************************************getters  ***********************************************/
/*
* @brief Get the total number of packets
* @return the total number of packets
*/
uint32_t ECPRI::getTotalNumOfPackets() const
{
    return totalNumOfPackets;
}