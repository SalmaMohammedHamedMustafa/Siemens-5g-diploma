#ifndef ORAN_HPP
#define ORAN_HPP

#include <cstdint>
#include <map>
#include <string>
#include <sys/types.h>
#include <vector>
class ORAN {
    public:
        /*
        * @brief Constructor
        * @param SCS: Subcarrier spacing
        */
        ORAN();
        /*
        * @brief Create an ORAN packet
        * @return ORAN packet
        */
        std::vector<uint8_t> createORANPacket();


    private:
    /*constans*/
    static const std::map<uint32_t, uint16_t> SCS2SlotsMap; //SCS to number of slots map
    static constexpr uint8_t firstByte = 0x00; //First byte of the ORAN packet
    static constexpr uint8_t BitsPerByte = 8; //Bits per byte
    static constexpr uint8_t ORANPacketHeaderBytes = 8; //ORAN packet header bytes
    static constexpr uint8_t IqSamplesPerRb = 12; //I/Q samples per resource block
    static constexpr uint8_t BitsPerSample = 32; //Bits per sample
    static constexpr uint16_t BytesPerRb = (IqSamplesPerRb * BitsPerSample) / BitsPerByte; //Bytes per resource block
    static constexpr uint8_t symbolPerSlot = 14; //Number of symbols per slot
    static constexpr uint8_t subFramePerFrame = 10; //Number of subframes per frame
    static constexpr uint8_t framesPerSec = 100; //Number of frames per second
    static constexpr uint8_t frameTimeMs = 10; //Frame time in ms

    /*variables*/
    std::string configFilePath = "../second_milestone.txt"; // path to the configuration file
    uint32_t LineRate; // in GB
    uint32_t CaptureSizeMs; // in ms
    uint32_t framesPerCapture; // number of frames in capture time
    uint32_t MaxPacketSize; // total packet size in bytes
    uint32_t Oran_SCS; // Subcarrier Spacing
    uint32_t Oran_MaxNrb; // Maximum number of resource blocks
    uint32_t Oran_NrbPerPacket; // Number of resource blocks per packet
    std::string Oran_PayloadType; // Random or fixed
    std::string Oran_Payload; // Filename of payload, if fixed
    uint16_t slotsPerSubFrame; // Number of slots
    uint32_t BytesPerPacket; // Number of bytes per packet
    uint32_t totalNumOfOranPackets; // Total number of ORAN packets
    uint64_t totalNumOfBytes; // Total number of bytes transmitted
    uint8_t frameId = 0; // Frame ID

    /*functions*/
    void printVariables(); //Print variables for debugging
    void parseConfigFile(); //parse the configuration file
    void calceBytesPerPacket(); //calculate the number of bytes per packet
    void calcTotalNumOfBytes(); //calculate the total number of bytes transmitted
    void calcTotalNumOfOranPackets(); //calculate the total number of ORAN packets
    void calcNumOfFrames(); //calculate the number of frames








};


#endif // ORAN_HPP