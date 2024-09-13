#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <cstdint>
#include <string>

class Ethernet 
{
    public:
        Ethernet(std::string configFilePath);
        void print() const;
        void generateFixed() const;

    private:
        std::string configFilePath;
        uint32_t LineRate; // in GB
        uint32_t CaptureSizeMs; // in ms
        uint32_t CaptureSizePacket; //numper of packets in capture time
        uint32_t MinNumOfIFGsPerPacket; 
        uint8_t IFGByte;
        uint64_t PreambleandSFD;
        uint64_t DestAddress; 
        uint64_t SourceAddress;
        uint32_t MaxPacketSize; // total packet size in bytes
        uint32_t BurstSize; // Number of ethernet packets in one burst
        uint32_t BurstPeriodicity_us; // The periodicity of the burst in microseconds

        void calcCaptureSizePacket();
        void parseConfigFile();
};

#endif // ETHERNET_HPP