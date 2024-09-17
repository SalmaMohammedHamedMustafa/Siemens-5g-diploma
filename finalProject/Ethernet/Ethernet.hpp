#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <cstdint>
#include <string>
#include "../CRC/CRC.hpp"

class Ethernet 
{
    public:
        Ethernet(std::string configFilePath);
        void generateFixed();

    private:
        CRC crc;
        static constexpr float bitsInByte = 8.0;
        static constexpr uint8_t IFGByte = 0x07;
        static constexpr uint64_t PreambleandSFD = 0xFB555555555555D5;
        static constexpr uint8_t PreambleNumBytes = 8;
        static constexpr uint8_t DestAddressNumBytes = 6;
        static constexpr uint8_t SourceAddressNumBytes = 6;
        static constexpr uint8_t EtherTypeNumBytes = 2;
        static constexpr uint8_t CRCNumBytes = 4;
        std::string configFilePath;
        std::string outputFilePath="../output.txt";
        uint32_t LineRate; // in GB
        uint32_t CaptureSizeMs; // in ms
        uint32_t CaptureSizePacket; //number of packets in capture time
        uint32_t MinNumOfIFGsPerPacket; 
        uint64_t DestAddress; 
        uint64_t SourceAddress;
        uint32_t MaxPacketSize; // total packet size in bytes
        uint32_t payloadSize;// payload size in bytes
        uint32_t BurstSize; // Number of ethernet packets in one burst
        uint32_t BurstPeriodicity_us; // The periodicity of the burst in microseconds
        uint32_t numberOfBurtsPerFrame; // Number of bursts in one frame
        double packetTransmissionTime_us; // Time to transmit one packet (in microseconds)
        double burstTransmissionTime_us; // Total time to transmit a burst (in microseconds)
        uint32_t totalPacketsPerFrame; // Total number of packets transmitted in one frame
        uint32_t IFGBytesAtEndOfBurst; // Number of IFG bytes at the end of the burst
        uint64_t totalNumOfBytes; // Total number of bytes transmitted
        double bytesPerMicrosecond; // Number of bytes transmitted per microsecond


        void calcIFGBytesAtEndOfBurst();
        void calcPacketTransmissionTime();
        void calcBurstTransmissionTime();
        void calcnumberOfBurtsPerFrame();
        void calcPacketsPerFrame();
        void calcCaptureSizePacket();
        void calcpayloadSize();
        void calcTotalNumOfBytes();
        void parseConfigFile();
        void calculateBytesPerMicrosecond();
        void writeToFile(const std::vector<uint8_t>& packet, std::ofstream& outputFile);
        std::vector<uint8_t>  generatePacket(uint32_t payloadSize);
        std::vector<uint8_t> generateIFG();
        void generateBurst(std::ofstream& outputFile);
        void writePacketsToFile(std::ofstream& outputFile);
        void hande4ByteAlignment(std::ofstream& outputFile);
        void calculations();
        void print() const;
};

#endif // ETHERNET_HPP
