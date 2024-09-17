#include "Ethernet.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>

Ethernet::Ethernet(std::string configFilePath): configFilePath(configFilePath){

    parseConfigFile();
    
}

void Ethernet::print() const {
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
    std::cout << "PacketTransmissionTime: " << PacketTransmissionTime << " us\n";
    std::cout << "BurstTransmissionTime: " << BurstTransmissionTime << " us\n";
    std::cout << "TotalPacketsPerFrame: " << TotalPacketsPerFrame << " packets\n";
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
            DestAddress = std::stoull(addressStr, nullptr, 0); // Convert from hex
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

void Ethernet::calcCaptureSizePacket() {
    CaptureSizePacket = ((static_cast<uint64_t>(CaptureSizeMs) * 1000) / BurstPeriodicity_us) * BurstSize;
}

void Ethernet::calcPacketTransmissionTime() {
    // Convert line rate to bits per second
    uint64_t lineRateBps = static_cast<uint64_t>(LineRate) * 1000000000;

    // Calculate packet size in bits
    uint64_t packetSizeBits = MaxPacketSize * bitsInByte;

    // Calculate the transmission time of one packet in microseconds
    PacketTransmissionTime = (static_cast<double>(packetSizeBits) / lineRateBps) * 1e6;
}

void Ethernet::calcBurstTransmissionTime() {
    // Calculate the time to transmit a burst (BurstSize packets)
    BurstTransmissionTime = BurstSize * (PacketTransmissionTime + (MinNumOfIFGsPerPacket * (MinNumOfIFGsPerPacket*bitsInByte) / (LineRate * 1000.0)));
}

void Ethernet::calcPacketsPerFrame() {
    // Each frame is CaptureSizeMs long
    double frameDurationUs = static_cast<double>(CaptureSizeMs) * 1000.0;

    // Calculate the total number of packets per frame
    TotalPacketsPerFrame = static_cast<uint32_t>(frameDurationUs / BurstPeriodicity_us) * BurstSize;
}

void Ethernet::generateFixed() 
{
    calcCaptureSizePacket();
    calcPacketTransmissionTime();
    calcBurstTransmissionTime();
    calcPacketsPerFrame();
}
