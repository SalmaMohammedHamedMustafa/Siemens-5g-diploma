#ifndef ETHERNET_HPP
#define ETHERNET_HPP

//#define MILESTONE_1  // Uncomment this for Milestone 1
#define MILESTONE_2  // Uncomment this for Milestone 2

#include <cstdint>
#include <string>
#include <vector>
#include "../CRC/CRC.hpp"



class Ethernet 
{
    public:
        /*
        @prief: Constructor
        @param: configFilePath: path to the configuration file
        */
        Ethernet(std::string configFilePath);

        /*
        @prief: function to generate Ethernet packets
        */
        void generate();
        

    private:
        CRC crc; // CRC object

        /*constants*/
        static constexpr float bitsInByte = 8.0;
        static constexpr uint8_t IFGByte = 0x07;
        static constexpr uint64_t PreambleandSFD = 0xFB555555555555D5;
        static constexpr uint8_t dummyData = 0x00;
        static constexpr uint8_t frameTime = 10; // in ms
        static constexpr uint8_t PreambleNumBytes = 8;
        static constexpr uint8_t DestAddressNumBytes = 6;
        static constexpr uint8_t SourceAddressNumBytes = 6;
        static constexpr uint8_t EtherTypeNumBytes = 2;
        static constexpr uint8_t CRCNumBytes = 4;
        static constexpr uint32_t MinPacketSize = 64;
        static constexpr uint32_t MinPayloadSize = MinPacketSize - (PreambleNumBytes + DestAddressNumBytes + SourceAddressNumBytes + EtherTypeNumBytes + CRCNumBytes);
        enum class PacketType
        {
            RandomPacket = 0,
            FixedPacket = 1
        }; // Enum to differentiate between the packet types

        std::string configFilePath; // path to the configuration file
        std::string outputFilePath="../output.txt"; // path to the output file
        uint32_t LineRate; // in GB
        uint32_t CaptureSizeMs; // in ms
        uint32_t CaptureSizePacket; //number of packets in capture time
        uint32_t MinNumOfIFGsPerPacket; 
        uint64_t DestAddress; // Destination MAC address
        uint64_t SourceAddress; // Source MAC address
        uint32_t MaxPacketSize; // total packet size in bytes
        uint32_t MaxPayloadSize ; // maximum payload size in bytes
        uint64_t totalNumOfBytes; // Total number of bytes transmitted
        double bytesPerMicrosecond; // Number of bytes transmitted per microsecond

        #ifdef MILESTONE_1
        // Milestone 1 configuration parameters
        uint32_t payloadSize;// payload size in bytes
        uint32_t BurstSize; // Number of ethernet packets in one burst
        uint32_t BurstPeriodicity_us; // The periodicity of the burst in microseconds
        uint32_t numberOfBurtsPerFrame; // Number of bursts in one frame
        double packetTransmissionTime_us; // Time to transmit one packet (in microseconds)
        double burstTransmissionTime_us; // Total time to transmit a burst (in microseconds)
        uint32_t totalPacketsPerFrame; // Total number of packets transmitted in one frame
        uint32_t IFGBytesAtEndOfBurst; // Number of IFG bytes at the end of the burst
        #endif

        #ifdef MILESTONE_2
        /* ORAN configuration parameters */
        uint32_t Oran_SCS; // Subcarrier Spacing
        uint32_t Oran_MaxNrb; // Maximum number of resource blocks
        uint32_t Oran_NrbPerPacket; // Number of resource blocks per packet
        std::string Oran_PayloadType; // Random or fixed
        std::string Oran_Payload; // Filename of payload, if fixed
        uint32_t numOfFrames; // number of frames 

        #endif


        /*calculations*/
        void calculations();
        void calculateBytesPerMicrosecond();
        void calcIFGBytesAtEndOfBurst();
        void calcPacketTransmissionTime();
        void calcBurstTransmissionTime();
        void calcnumberOfBurtsPerFrame();
        void calcPacketsPerFrame();
        void calcCaptureSizePacket();
        void calcpayloadSize();
        void calcTotalNumOfBytes();
        void calcMaxPayloadSize();
        void calcNumOfFrames();

        /*general functions*/
        void writeToFile(const std::vector<uint8_t>& bytes, std::ofstream& outputFile); // write a vector of bytes to the output file   
        std::vector<uint8_t> generateIFG(); // generate the IFG bytes
        void print() const; //print the configuration parameters for debugging
        void hande4ByteAlignment(std::ofstream& outputFile); // handle the 4 byte alignment and add the IFG padding if not aligned

    

        /*Random mode functions*/
        void generateRandom(); // generate the packets in Random mode and write them to the output file
        void parseConfigFile(); //parse the configuration file
        void generateBurst(std::ofstream& outputFile); // generate a burst of packets
        void writeDummyPacketsToFile(std::ofstream& outputFile); // write the packets to the output file
        std::vector<uint8_t> generatePacket(uint32_t payloadSize, PacketType PacketType); // generate a packet with a given payload size

        /*fixed mode functions*/


        /*packet parts*/
        void addPreambleandSFD(std::vector<uint8_t> &packet); // add the preamble and SFD to the packet
        void addDestAddress(std::vector<uint8_t> &packet); // add the destination MAC address to the packet
        void addSourceAddress(std::vector<uint8_t> &packet); // add the source MAC address to the packet
        void addEtherType(std::vector<uint8_t> &packet); // add the EtherType to the packet
        void addDummyPayload(std::vector<uint8_t> &packet,uint32_t payloadSize, uint8_t dummyData); // generate a dummy payload
        void checkPacketSize(uint32_t &payloadSize); // check the packet size and adjust it if needed
        void addFixedPayload(std::vector<uint8_t> &packet, uint32_t payloadSize, std::string payload); // add a fixed payload to the packet
};

#endif // ETHERNET_HPP
