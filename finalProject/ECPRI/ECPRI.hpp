#ifndef ECPRI_HPP
#define ECPRI_HPP

#include <cstdint>
#include <vector>


class ECPRI {
    public:
        /*
        * @brief Constructor
        * @param SCS: Subcarrier spacing
        */
        ECPRI(uint32_t SCS, uint16_t MaxNrb, uint16_t NrbPerPacket, uint32_t MaxECPRIPacketSize, uint32_t CaptureTime);
        /*
        * @brief Create an ECPRI packet
        * @return ECPRI packet
        */
        std::vector<uint8_t> createECPRIPacket();
    
    private:
    /*constans*/
    static constexpr uint8_t ECPRIPacketHeaderBytes = 8; //ECPRI packet header bytes
    static constexpr uint8_t firstByte = 0x00; //First byte of the ECPRI packet
    static constexpr uint8_t message = 0x00; //Message type
    static constexpr uint8_t PC_RTC = 0x00; //Fixed

    /*variables*/
    uint32_t SCS; //Subcarrier spacing
    uint16_t MaxNrb; //Maximum number of resource blocks in the carrier
    uint16_t NrbPerPacket; //Number of resource blocks per packet
    uint16_t MaxECPRIPacketSize; //Maximum ECPRI packet size
    uint32_t CaptureTime; //Capture timecode 
    uint16_t MaxORANPacketSize; //Maximum ORAN packet size
    uint16_t payload; //Payload size
    uint16_t seqId; //Sequence ID


    /*functions*/
    void printVariables(); //Print variables for debugging

};

#endif // ECPRI_HPP