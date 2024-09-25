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
        ORAN(uint64_t MaxPacketSize);
        ORAN();
        /*
        * @brief Create an ORAN packet
        * @return ORAN packet
        */
        std::vector<uint8_t> createORANPacket();

        /*getters*/
        uint32_t getTotalNumOfPackets() const; //get the total number of packets 

        /*setters*/
        void setMaxPacketSize(uint64_t MaxPacketSize); //set the maximum packet size


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
    static constexpr uint8_t sectionId = 0x00; //Section ID
    static constexpr uint8_t syminc = 0x00; //Symbol increment
    static constexpr uint8_t startPrbu = 0x00; //Start PRBU

    static constexpr uint8_t maxFrameId = 99; //Maximum frame ID
    static constexpr uint8_t maxSubframeId = 9; //Maximum subframe ID
    static constexpr uint8_t maxSlotId = 7; //Maximum slot ID
    static constexpr uint8_t maxSymbolId = 13; //Maximum symbol ID

    static constexpr uint8_t maxNrbPerPacket = 30; //Maximum number of resource blocks per packet to avoid fragmentation



    /*variables*/
    std::string configFilePath = "../second_milstone.txt"; // path to the configuration file
    /*config variables*/
    uint32_t LineRate; // in GB
    uint32_t CaptureSizeMs; // in ms
    uint32_t Oran_SCS; // Subcarrier Spacing
    uint32_t Oran_MaxNrb; // Maximum number of resource blocks
    uint32_t Oran_NrbPerPacket; // Number of resource blocks per packet
    std::string Oran_PayloadType; // Random or fixed

    std::string Oran_Payload ; // Filename of payload, if fixed

    uint16_t slotsPerSubFrame; // Number of slots
    uint32_t BytesPerPacket; // Number of bytes per packet
    uint32_t BytesPerPayload; // Number of bytes per payload
    uint32_t framesPerCapture; // number of frames in capture time
    uint64_t MaxPacketSize; // total packet size in bytes 

    uint32_t packetsPerSymbol; // Number of packets per symbol
    uint32_t packetsPerSlot; // Number of packets per slot
    uint32_t packetsPerSubFrame; // Number of packets per subframe
    uint32_t packetsPerFrame; // Number of packets per frame
    uint32_t packetsPerSec; // Number of packets per second

    uint32_t totalNumOfOranPackets; // Total number of ORAN packets
    uint64_t totalNumOfBytes; // Total number of bytes transmitted

    uint8_t frameId = 0; // Frame ID
    uint8_t subframeId = 0; // Subframe ID
    uint8_t slotId = 0; // Slot ID
    uint8_t symbolId = 0; // Symbol ID

    std::streampos lastFilePosition = 0;  // To store the file position
    size_t bytesPushedTotal = 0;          // To store the total number of bytes pushed across calls


    /*functions*/
    /*genral functions*/
    void printVariables(); //Print variables for debugging
    void printPacket(const std::vector<uint8_t> &packet); //Print the ORAN packet
    void parseConfigFile(); //parse the configuration file
    void findSlotsPerSubFrame(); //find the number of slots per subframe
    void parseIQData(std::vector<uint8_t> &packet); //parse the IQ data from the file
    void generateRandomPayload(std::vector<uint8_t> &packet); //generate random payload
    int16_t convertTo16Bit(int value); 
    void handleFragmentation(); 

    /*calculations*/
    void calculate(); //calculate all the variables
    void calceBytesPerPacket(); //calculate the number of bytes per packet
    void calcBytesPerPayload(); //calculate the number of bytes per payload
    void calcTotalNumOfBytes(); //calculate the total number of bytes transmitted
    void calcPacketsPerSymbol(); //calculate the number of packets per symbol 
    void calcPacketsPerSlot(); //calculate the number of packets per slot
    void calcPacketsPerSubFrame(); //calculate the number of packets per subframe
    void calcPacketsPerFrame(); //calculate the number of packets per frame
    void calcPacketsPerSec(); //calculate the number of packets per second
    void calcTotalNumOfOranPackets(); //calculate the total number of ORAN packets
    void calcNumOfFrames(); //calculate the number of frames

    /*update IDs functions*/
    void updateFrameId(); //update the frame ID
    void updateSubframeId(); //update the subframe ID
    void updateSlotId(); //update the slot ID
    void updateSymbolId(); //update the symbol ID





    







};


#endif // ORAN_HPP