#include "Ethernet.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>

Ethernet::Ethernet(std::string configFilePath): configFilePath(configFilePath) {
    parseConfigFile();
    calculateBytesPerMicrosecond();
}

void Ethernet::calculateBytesPerMicrosecond() {
    // Calculate the number of bytes transmitted per microsecond
    bytesPerMicrosecond = (static_cast<double>(LineRate) * 1e9) / bitsInByte / 1e6;
}

void Ethernet::calcPacketTransmissionTime() {
    // Calculate the transmission time of one packet in microseconds using bytesPerMicrosecond
    packetTransmissionTime_us = MaxPacketSize / bytesPerMicrosecond;
}

void Ethernet::calcBurstTransmissionTime() {
    double totalIFGTime_us = (BurstSize - 1) * (MinNumOfIFGsPerPacket / bytesPerMicrosecond); // 12 bytes IFG after each packet except the last
    burstTransmissionTime_us = (BurstSize * packetTransmissionTime_us) + totalIFGTime_us;
}

void Ethernet::calcpayloadSize() {
    payloadSize = MaxPacketSize - (PreambleNumBytes + DestAddressNumBytes + SourceAddressNumBytes + EtherTypeNumBytes + CRCNumBytes);
}
void Ethernet::calcIFGBytesAtEndOfBurst() {
    // Calculate the burst IFG time in microseconds
    int burstIFGtime = BurstPeriodicity_us - burstTransmissionTime_us;

    // Calculate the number of IFG bytes at the end of the burst
    IFGBytesAtEndOfBurst = burstIFGtime * bytesPerMicrosecond;
}
void Ethernet::calcnumberOfBurtsPerFrame() {
    // Calculate the number of bursts in one frame
    numberOfBurtsPerFrame = static_cast<uint32_t>(CaptureSizeMs * 1000.0 / BurstPeriodicity_us);
}
void Ethernet::calcPacketsPerFrame() {
    // Calculate the total number of packets per frame
    totalPacketsPerFrame =numberOfBurtsPerFrame*  BurstSize;
}

void Ethernet::calcCaptureSizePacket() {
    CaptureSizePacket = ((static_cast<uint64_t>(CaptureSizeMs) * 1000) / BurstPeriodicity_us) * BurstSize;
}

void Ethernet::calcTotalNumOfBytes() {
    // Calculate the total number of bytes transmitted in one frame
    uint64_t NumOfBytesInBurst = MaxPacketSize * BurstSize + MinNumOfIFGsPerPacket * (BurstSize - 1) + IFGBytesAtEndOfBurst;
    totalNumOfBytes = NumOfBytesInBurst * numberOfBurtsPerFrame;
}

void Ethernet::generateFixed() {

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

void Ethernet::calculations() {
    calcCaptureSizePacket();
    calcPacketTransmissionTime();
    calcBurstTransmissionTime();
    calcIFGBytesAtEndOfBurst();
    calcnumberOfBurtsPerFrame();
    calcPacketsPerFrame();
    calcpayloadSize();
    calcTotalNumOfBytes();
    print();
}

void Ethernet::generateBurst(std::ofstream& outputFile) {
    writePacketsToFile(outputFile);
    std::vector<uint8_t> IFGBytesAtEndOfBurstVector(IFGBytesAtEndOfBurst, IFGByte);
    writeToFile(IFGBytesAtEndOfBurstVector, outputFile);
}

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

void Ethernet::writePacketsToFile(std::ofstream& outputFile) {

    for (uint32_t i = 0; i < BurstSize; ++i)
    {
        std::vector<uint8_t> packet = generatePacket(payloadSize);
        writeToFile(packet, outputFile);
        if (i != BurstSize - 1) {
            std::vector<uint8_t> IFG = generateIFG();
            writeToFile(IFG, outputFile);
        }
    }

}
std::vector<uint8_t> Ethernet::generateIFG() {
    std::vector<uint8_t> IFG;
    for (int i = 0; i < MinNumOfIFGsPerPacket; ++i) {
        IFG.push_back(IFGByte);
    }
    return IFG;
}


void Ethernet::writeToFile(const std::vector<uint8_t>& packet, std::ofstream& outputFile) {
    // Dump packet data in 4-byte aligned format
    for (size_t j = 0; j < packet.size(); ++j) {
        if (j % 4 == 0) {
            outputFile << "\n";
        }
        outputFile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(packet[j]) << " ";
    }
    outputFile << std::dec;

}


void Ethernet::print() const 
{
    std::cout << "LineRate: " << LineRate << " GB\n";
    std::cout << "CaptureSizeMs: " << CaptureSizeMs << " ms\n";
    std::cout << "CaptureSizePacket: " << CaptureSizePacket << " Packet\n";
    std::cout << "MinNumOfIFGsPerPacket: " << MinNumOfIFGsPerPacket <<"\n";
    std::cout<< "IFGByte: 0x" << std::hex << static_cast<int>(IFGByte) << std::dec <<"\n";
    std::cout << "PreambleandSFD: 0x" << std::hex << PreambleandSFD << std::dec <<"\n";
    std::cout << "DestAddress: 0x" << std::hex << DestAddress << std::dec <<"\n";
    std::cout << "SourceAddress: 0x" << std::hex << SourceAddress << std::dec << "\n";
    std::cout << "MaxPacketSize: " << MaxPacketSize << " bytes" << "\n";
    std::cout << "BurstSize: " << BurstSize <<"\n";
    std::cout << "BurstPeriodicity: " << BurstPeriodicity_us << " us\n";
    std::cout << "PacketTransmissionTime: " << packetTransmissionTime_us << " us\n";
    std::cout << "BurstTransmissionTime: " << burstTransmissionTime_us << " us\n";
    std::cout << "TotalPacketsPerFrame: " << totalPacketsPerFrame << " packets\n";
    std::cout << "IFGBytesAtEndOfBurst: " << IFGBytesAtEndOfBurst << " bytes\n";
    std::cout << "number of Burts Per Frame: " << numberOfBurtsPerFrame << "\n";
    std::cout << "TotalNumOfBytes: " << totalNumOfBytes << " bytes\n";
}

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
        } else if (line.find("Eth.BurstSize") == 0) {
            BurstSize = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Eth.BurstPeriodicity_us") == 0) {
            BurstPeriodicity_us = std::stoul(line.substr(line.find('=') + 1));
        }
    }

    configFile.close();
}






std::vector<uint8_t> Ethernet::generatePacket(uint32_t payloadSize) {
    std::vector<uint8_t> packet;

    // Add Preamble and SFD (8 bytes)
    for (int i = 7; i >= 0; --i) {
        packet.push_back((PreambleandSFD >> (i * 8)) & 0xFF);
    }

    // Add Destination MAC Address (6 bytes)
    for (int i = 5; i >= 0; --i) {
        packet.push_back((DestAddress >> (i * 8)) & 0xFF);
    }

    // Add Source MAC Address (6 bytes)
    for (int i = 5; i >= 0; --i) {
        packet.push_back((SourceAddress >> (i * 8)) & 0xFF);
    }

    // Add Type/Length (2 bytes, assuming EtherType for simplicity)
    uint16_t etherType = 0x0800; // Example: IPv4 EtherType (0x0800)
    packet.push_back((etherType >> 8) & 0xFF);  // High byte
    packet.push_back(etherType & 0xFF);         // Low byte


    if (payloadSize < 46) payloadSize = 46;    // Minimum payload size is 46 bytes
    if (payloadSize > 1500) payloadSize = 1500; // Maximum Ethernet payload size is 1500 bytes

    for (uint32_t i = 0; i < payloadSize; ++i) {
        packet.push_back(0x00); // Example payload data (can be replaced with actual data)
    }

    // Apply CRC and append it to the packet (4 bytes)
    crc.applyCRC(packet);


    return packet;
}
