#include "Ethernet.hpp"
#include "../ECPRI/ECPRI.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>

/*
@prief: Constructor
@param: configFilePath: path to the configuration file
*/
Ethernet::Ethernet(std::string configFilePath): configFilePath(configFilePath) {
    parseConfigFile(); // Parse the configuration file 

}

/*
@prief: function to generate Ethernet packets
*/
void Ethernet::generate() {
#ifdef MILESTONE_1
    generateRandom();
#endif
#ifdef MILESTONE_2
    // Add your code here
#endif

}


/**************************************************** calculations functions *********************************************************************/
/*
@prief: function to calculate the number of bytes transmitted per microsecond 
*/
void Ethernet::calculateBytesPerMicrosecond() {
    bytesPerMicrosecond = (static_cast<double>(LineRate) * 1e9) / bitsInByte / 1e6;
}

/*
@prief: function to calculate the maximum payload size
*/
void Ethernet::calcMaxPayloadSize() {
    MaxPayloadSize = MaxPacketSize - (PreambleNumBytes + DestAddressNumBytes + SourceAddressNumBytes + EtherTypeNumBytes + CRCNumBytes);
}

#ifdef MILESTONE_1
/*
@prief: function to calculate the time to transmit one packet
*/
void Ethernet::calcPacketTransmissionTime() {
    packetTransmissionTime_us = MaxPacketSize / bytesPerMicrosecond;
}

/*
@prief: function to calculate the transmission time of in one burst
*/
void Ethernet::calcBurstTransmissionTime() {
    double totalIFGTime_us = (BurstSize - 1) * (MinNumOfIFGsPerPacket / bytesPerMicrosecond); // 12 bytes IFG after each packet except the last
    burstTransmissionTime_us = (BurstSize * packetTransmissionTime_us) + totalIFGTime_us;
}

/*
@prief: function to calculate the payload size
*/
void Ethernet::calcpayloadSize() {
    payloadSize = MaxPayloadSize;
}

/*
@prief: function to calculate the number of IFG bytes at the end of the burst
*/
void Ethernet::calcIFGBytesAtEndOfBurst() {
    int burstIFGtime = BurstPeriodicity_us - burstTransmissionTime_us; // Calculate the burst IFG time in microseconds
    IFGBytesAtEndOfBurst = burstIFGtime * bytesPerMicrosecond;
}

/*
@prief: function to calculate the number of bursts in one frame
*/
void Ethernet::calcnumberOfBurtsPerFrame() {
    numberOfBurtsPerFrame = static_cast<uint32_t>(CaptureSizeMs * 1000.0 / BurstPeriodicity_us);
}

/*
@prief: function to calculate the total number of packets per frame
*/
void Ethernet::calcPacketsPerFrame() {
    totalPacketsPerFrame =numberOfBurtsPerFrame*  BurstSize;
}

/*
@prief: function to calculate the number of packets in capture time
*/
void Ethernet::calcCaptureSizePacket() {
    CaptureSizePacket = ((static_cast<uint64_t>(CaptureSizeMs) * 1000) / BurstPeriodicity_us) * BurstSize;
}

/*
@prief: function to calculate the total number of bytes transmitted
*/
void Ethernet::calcTotalNumOfBytes() {
    uint64_t NumOfBytesInBurst = MaxPacketSize * BurstSize + MinNumOfIFGsPerPacket * (BurstSize - 1) + IFGBytesAtEndOfBurst;
    totalNumOfBytes = NumOfBytesInBurst * numberOfBurtsPerFrame;
}
#endif


#ifdef MILESTONE_2
/*
@prief: function to calculate the number of frames
*/
void Ethernet::calcNumOfFrames() {
    numOfFrames = CaptureSizeMs / frameTime;
}
#endif


/*
@prief: function to do all the calculations and print the results
*/
void Ethernet::calculations() {
    calculateBytesPerMicrosecond();
    calcMaxPayloadSize();
#ifdef MILESTONE_1
    calcMaxPayloadSize();
    calcCaptureSizePacket();
    calcPacketTransmissionTime();
    calcBurstTransmissionTime();
    calcIFGBytesAtEndOfBurst();
    calcnumberOfBurtsPerFrame();
    calcPacketsPerFrame();
    calcTotalNumOfBytes();
#endif
#ifdef MILESTONE_2
    calcNumOfFrames();
#endif 

    print();
}
/*****************************************************************************************************************************************/

/***************************************************** Fixed mode funcions ****************************************************************/
#ifdef MILESTONE_1
/*
@prief: function to generate the packets in fixed mode and write them to the output file
*/
void Ethernet::generateRandom() {
    calculations();
    // Open the file once before the loop with truncation to clear it
    std::ofstream outputFile(outputFilePath, std::ios::out | std::ios::trunc);
    
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file for writing: output.txt" << std::endl;
        return;
    }

    for (uint32_t i = 0; i < numberOfBurtsPerFrame; ++i) {
        generateBurst(outputFile);
    }

    hande4ByteAlignment(outputFile);

    // Close the file after all packets are written
    outputFile.close();
}


/*
@prief: function to generate a burst of packets used in fixed mode
@param: outputFile: the output file stream
*/
void Ethernet::generateBurst(std::ofstream& outputFile) {
    writeDummyPacketsToFile(outputFile);
    std::vector<uint8_t> IFGBytesAtEndOfBurstVector(IFGBytesAtEndOfBurst, IFGByte);
    writeToFile(IFGBytesAtEndOfBurstVector, outputFile);
}


/*
@prief: function to write the dummy packets to the output file
@param: outputFile: the output file stream
*/
void Ethernet::writeDummyPacketsToFile(std::ofstream& outputFile) {

    for (uint32_t i = 0; i < BurstSize; ++i)
    {
        std::vector<uint8_t> packet = generatePacket(payloadSize, PacketType::RandomPacket);
        writeToFile(packet, outputFile);
        if (i != BurstSize - 1) {
            std::vector<uint8_t> IFG = generateIFG();
            writeToFile(IFG, outputFile);
        }
    }

}


/*****************************************************************************************************************************************/
#endif

/************************************************** general functions **************************************************************/
/*
@prief: function to generate the IFG bytes and return them as a vector
*/
std::vector<uint8_t> Ethernet::generateIFG() {
    std::vector<uint8_t> IFG;
    for (int i = 0; i < MinNumOfIFGsPerPacket; ++i) {
        IFG.push_back(IFGByte);
    }
    return IFG;
}

/*
@prief: function to write a vector of bytes to the output file
@param: bytes: the vector of bytes to be written
@param: outputFile: the output file stream
*/
void Ethernet::writeToFile(const std::vector<uint8_t>& bytes, std::ofstream& outputFile) {
    // Dump packet data in 4-byte aligned format
    for (size_t j = 0; j < bytes.size(); ++j) {
        if (j % 4 == 0) {
            outputFile << "\n";
        }
        outputFile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[j]) << " ";
    }
    outputFile << std::dec;

}

/*
@prief: function to handle the 4 byte alignment and add the IFG padding if not aligned
@param: outputFile: the output file stream
*/
void Ethernet::hande4ByteAlignment(std::ofstream& outputFile) {
    if (totalNumOfBytes % 4 != 0) {
        // Calculate the number of bytes needed for alignment
        uint32_t alignmentBytes = 4 - (totalNumOfBytes % 4);

        // Add IFG padding for alignment
        if (alignmentBytes > 0) {
            std::vector<uint8_t> IFGPadding(alignmentBytes, IFGByte);
            writeToFile(IFGPadding, outputFile);
        }
        
    }
}

/*
@prief: function to print the configuration parameters for debugging
*/
void Ethernet::print() const 
{
    std::cout<<"\n";
    std::cout <<"******************************************************************************************\n";
    std::cout << "LineRate: " << LineRate << " GB\n";
    std::cout << "CaptureSizeMs: " << CaptureSizeMs << " ms\n";
    std::cout << "CaptureSizePacket: " << CaptureSizePacket << " Packet\n";
    std::cout << "MinNumOfIFGsPerPacket: " << MinNumOfIFGsPerPacket <<"\n";
    std::cout<< "IFGByte: 0x" << std::hex << static_cast<int>(IFGByte) << std::dec <<"\n";
    std::cout << "PreambleandSFD: 0x" << std::hex << PreambleandSFD << std::dec <<"\n";
    std::cout << "DestAddress: 0x" << std::hex << DestAddress << std::dec <<"\n";
    std::cout << "SourceAddress: 0x" << std::hex << SourceAddress << std::dec << "\n";
    std::cout << "MaxPacketSize: " << MaxPacketSize << " bytes" << "\n";
    #ifdef MILESTONE_1
    std::cout << "BurstSize: " << BurstSize <<"\n";
    std::cout << "BurstPeriodicity: " << BurstPeriodicity_us << " us\n";
    std::cout << "PacketTransmissionTime: " << packetTransmissionTime_us << " us\n";
    std::cout << "BurstTransmissionTime: " << burstTransmissionTime_us << " us\n";
    std::cout << "TotalPacketsPerFrame: " << totalPacketsPerFrame << " packets\n";
    std::cout << "IFGBytesAtEndOfBurst: " << IFGBytesAtEndOfBurst << " bytes\n";
    std::cout << "number of Burts Per Frame: " << numberOfBurtsPerFrame << "\n";
    std::cout << "TotalNumOfBytes: " << totalNumOfBytes << " bytes\n";
    #endif
    std::cout << "frameTime: " << frameTime << " ms\n";
}

/*
@prief: function to parse the configuration file
*/
void Ethernet::parseConfigFile()
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
        } else if (line.find("Eth.MinNumOfIFGsPerPacket") == 0) {
            MinNumOfIFGsPerPacket = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Eth.DestAddress") == 0) {
            std::string addressStr = line.substr(line.find('=') + 1);
            DestAddress = std::stoull(addressStr, nullptr, 0); // Ensure it's handling 48 bits
        } else if (line.find("Eth.SourceAddress") == 0) {
            std::string addressStr = line.substr(line.find('=') + 1);
            SourceAddress = std::stoull(addressStr, nullptr, 0); // Convert from hex
        } else if (line.find("Eth.MaxPacketSize") == 0) {
            MaxPacketSize = std::stoul(line.substr(line.find('=') + 1));
        } 
        #ifdef MILESTONE_1
        else if (line.find("Eth.BurstSize") == 0) {
            BurstSize = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Eth.BurstPeriodicity_us") == 0) {
            BurstPeriodicity_us = std::stoul(line.substr(line.find('=') + 1));
        }
        #endif

        #ifdef MILESTONE_2
        // ORAN Configuration Parsing (for Milestone 2)
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
        #endif
    }

    configFile.close();
}
/*****************************************************************************************************************************************/





/************************************************** packet generation functions **************************************************************/
/*
@prief: function to generate a packet with a given payload size
@param: payloadSize: the size of the payload in bytes
@param: PacketType: the type of the packet (Dummy or Random)
@return: the generated packet as a vector of bytes
*/
std::vector<uint8_t> Ethernet::generatePacket(uint32_t payloadSize, PacketType PacketType) {
    // Create a vector to store the packet
    std::vector<uint8_t> packet;
    // Add Preamble and SFD (8 bytes)
    addPreambleandSFD(packet);
    // Add Destination Address (6 bytes)
    addDestAddress(packet);
    // Add Source Address (6 bytes)
    addSourceAddress(packet);
    // Add EtherType (2 bytes)
    addEtherType(packet);
    // Check the packet size
    checkPacketSize(payloadSize);
    // Add Payload (variable size)
    if (PacketType == PacketType::RandomPacket)
    {
        addDummyPayload(packet, payloadSize, dummyData);
    }
    else if (PacketType == PacketType::RandomPacket)
    {
        // Add fixed payload
    }
    // Apply CRC and append it to the packet (4 bytes)
    crc.applyCRC(packet);
    return packet;
}

void Ethernet::addPreambleandSFD(std::vector<uint8_t>& packet) {
    for (int i = PreambleNumBytes - 1; i >= 0; --i) {
        packet.push_back((PreambleandSFD >> (i * 8)) & 0xFF);
    }
}

void Ethernet::addDestAddress(std::vector<uint8_t>& packet) {
    for (int i = DestAddressNumBytes - 1; i >= 0; --i) {
        packet.push_back((DestAddress >> (i * 8)) & 0xFF);
    }
}

void Ethernet::addSourceAddress(std::vector<uint8_t>& packet) {
    for (int i = SourceAddressNumBytes - 1; i >= 0; --i) {
        packet.push_back((SourceAddress >> (i * 8)) & 0xFF);
    }
}

void Ethernet::addEtherType(std::vector<uint8_t>& packet) {
    uint16_t etherType = 0x0800; // Example: IPv4 EtherType (0x0800)
    packet.push_back((etherType >> 8) & 0xFF);  // High byte
    packet.push_back(etherType & 0xFF);         // Low byte
}

void Ethernet::checkPacketSize(uint32_t& payloadSize) {
    if (payloadSize < MinPayloadSize) payloadSize = MinPayloadSize;    // Minimum payload size is 46 bytes
    if (payloadSize > MaxPayloadSize) payloadSize = MaxPayloadSize; // Maximum Ethernet payload size is 1500 bytes
}



void Ethernet::addDummyPayload(std::vector<uint8_t>& packet, uint32_t payloadSize, uint8_t dummyData) {
    for (uint32_t i = 0; i < payloadSize; ++i) {
        packet.push_back(dummyData);
    }
}

void Ethernet::addFixedPayload(std::vector<uint8_t>& packet, uint32_t payloadSize, std::string payload) {
    ECPRI ecpri(Oran_SCS, Oran_MaxNrb, Oran_NrbPerPacket, MaxPayloadSize, numOfFrames);
    
}
/*****************************************************************************************************************************************/