#ifndef ETHERNET_HPP
#define ETHERNET_HPP

//#define MILESTONE_1  // Uncomment this for Milestone 1
#define MILESTONE_2  // Uncomment this for Milestone 2

#include <cstdint>
#include <string>
#include <vector>
#include "../CRC/CRC.hpp"
#ifdef MILESTONE_2
#include "../ECPRI/ECPRI.hpp"
#endif



class Ethernet 
{
    public:
        /*
        @prief: Constructor
        */
        Ethernet();

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
        uint32_t payloadSize;// payload size in bytes


        #ifdef MILESTONE_1
        // Milestone 1 configuration parameters
        std::string configFilePath = "../first_milestone.txt"; // path to the configuration file
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
        std::string configFilePath = "../second_milstone.txt"; // path to the configuration file
        uint32_t totalNumOfPackets; // Total number of ORAN packets
        ECPRI ecpri; // ECPRI instance

        #endif


        /*calculations*/
        void calculations(); // do all the calculations and print the results
        void calculateBytesPerMicrosecond();
        void calcMaxPayloadSize();
        void calcpayloadSize();
        #ifdef MILESTONE_1
        void calcCaptureSizePacket();
        void calcPacketTransmissionTime();
        void calcBurstTransmissionTime();
        void calcIFGBytesAtEndOfBurst();
        void calcnumberOfBurtsPerFrame();
        void calcPacketsPerFrame();
        void calcTotalNumOfBytes();
        #endif

        /*general functions*/
        void writeToFile(const std::vector<uint8_t>& bytes, std::ofstream& outputFile); // write a vector of bytes to the output file   
        std::vector<uint8_t> generateIFG(); // generate the IFG bytes
        void print() const; //print the configuration parameters for debugging
        void hande4ByteAlignment(std::vector<uint8_t>& packet); // handle the 4 byte alignment and add the IFG padding if not aligned
        void parseConfigFile(); //parse the configuration file

    
        #ifdef MILESTONE_1
        /*Burst mode functions*/
        void generateBurstMode(); // generate the packets in Random mode and write them to the output file
        void generateBurst(std::ofstream& outputFile); // generate a burst of packets
        void writeDummyPacketsToFile(std::ofstream& outputFile); // write the packets to the output file
        std::vector<uint8_t> generatePacketRandom(); // generate a packet with a given payload size
        void addDummyPayload(std::vector<uint8_t> &packet, uint8_t dummyData); // generate a dummy payload
        #endif

        #ifdef MILESTONE_2
        /*full ORAN and eCPRI mode functions*/
        void generateFull(); // generate the packets in Fixed mode and write them to the output file
        void generatePacketsFull(std::ofstream& outputFile); // generate all packets in Full mode
        std::vector<uint8_t> generatePacketFull(); // generate a packet 
        void addEcpriPayload(std::vector<uint8_t> &packet); // add the ECPRI payload to the packet
        #endif

        /*packet parts*/
        void addPreambleandSFD(std::vector<uint8_t> &packet); // add the preamble and SFD to the packet
        void addDestAddress(std::vector<uint8_t> &packet); // add the destination MAC address to the packet
        void addSourceAddress(std::vector<uint8_t> &packet); // add the source MAC address to the packet
        void addEtherType(std::vector<uint8_t> &packet); // add the EtherType to the packet
        void checkPacketSize(uint32_t &payloadSize); // check the packet size and adjust it if needed
};

#endif // ETHERNET_HPP