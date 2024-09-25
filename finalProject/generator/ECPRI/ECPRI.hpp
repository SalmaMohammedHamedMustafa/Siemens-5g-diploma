#ifndef ECPRI_HPP
#define ECPRI_HPP

#include <cstdint>
#include <vector>
#include "../ORAN/ORAN.hpp"

class ECPRI {
    public:
        /*
        * @brief Constructor
        * @param SCS: Subcarrier spacing
        */
        ECPRI(uint64_t MaxECPRIPacketSize);

        ECPRI();    
        /*
        * @brief Create an ECPRI packet
        * @return ECPRI packet
        */
        std::vector<uint8_t> createECPRIPacket();

        /*getters*/
        uint32_t getTotalNumOfPackets() const; //Get the total number of packets

        /*setters*/
        void setMaxPacketSize(uint64_t MaxECPRIPacketSize); //Set the maximum packet size
    
    private:
    /*constans*/
    static constexpr uint8_t ECPRIPacketHeaderBytes = 8; //ECPRI packet header bytes
    static constexpr uint8_t firstByte = 0x00; //First byte of the ECPRI packet
    static constexpr uint8_t message = 0x00; //Message type
    static constexpr uint16_t PC_RTC = 0x00; //Fixed

    /*variables*/
    uint16_t MaxECPRIPacketSize; //Maximum ECPRI packet size
    uint16_t MaxORANPacketSize; //Maximum ORAN packet size
    uint16_t payload; //Payload size
    uint32_t totalNumOfPackets; //Total number of packets transmitted
    /*starts from 0 to 255 and resets to 0 after 255*/
    uint16_t seqId; //Sequence ID
    ORAN oran; //ORAN instance


    /*functions*/
    void printVariables(); //Print variables for debugging
    void printPacket(const std::vector<uint8_t>& packet); //Print the packet
    void updateSeqId(); //Update the sequence ID
    std::vector<uint8_t> getOranPacket(ORAN &oran); //Get the payload

    /*setters*/
    void setTotalNumOfPackets(uint32_t totalNumOfPackets); //Set the total number of packets



};

#endif // ECPRI_HPP