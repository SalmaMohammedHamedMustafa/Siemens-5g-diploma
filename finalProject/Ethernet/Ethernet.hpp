#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <cstdint>
#include <string>

class Ethernet 
{
    public:
        Ethernet(std::string configFilePath);
        void print() const;
        void generateFixed();

    private:
        std::string configFilePath;
        uint32_t LineRate; // in GB
        uint32_t CaptureSizeMs; // in ms
        uint32_t CaptureSizePacket; //number of packets in capture time
        uint32_t MinNumOfIFGsPerPacket; 
        const uint8_t IFGByte = 0x07;
        const uint64_t PreambleandSFD = 0xFB555555555555D5;
        uint64_t DestAddress; 
        uint64_t SourceAddress;
        uint32_t MaxPacketSize; // total packet size in bytes
        uint32_t BurstSize; // Number of ethernet packets in one burst
        uint32_t BurstPeriodicity_us; // The periodicity of the burst in microseconds
        double PacketTransmissionTime; // Time to transmit one packet (in microseconds)
        double BurstTransmissionTime; // Total time to transmit a burst (in microseconds)
        uint32_t TotalPacketsPerFrame; // Total number of packets transmitted in one frame

        void calcPacketTransmissionTime();
        void calcBurstTransmissionTime();
        void calcPacketsPerFrame();
        void calcCaptureSizePacket();
        void parseConfigFile();
};

#endif // ETHERNET_HPP
