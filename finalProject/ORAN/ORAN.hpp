#ifndef ORAN_HPP
#define ORAN_HPP

#include <cstdint>
#include <map>
#include <sys/types.h>
#include <vector>
class ORAN {
    public:
        /*
        * @brief Constructor
        * @param SCS: Subcarrier spacing
        */
        ORAN(uint32_t SCS, uint16_t MaxNrb, uint16_t NrbPerPacket, uint32_t MaxORANPacketSize, uint16_t numOfFrames);
        /*
        * @brief Create an ORAN packet
        * @return ORAN packet
        */
        std::vector<uint8_t> createORANPacket();


    private:
    /*constans*/
    static const std::map<uint32_t, uint16_t> SCS2SlotsMap; //SCS to number of slots map
    static constexpr uint8_t ORANPacketHeaderBytes = 8; //ORAN packet header bytes
    /*variables*/
    uint32_t SCS=0; //Subcarrier spacing
    uint16_t numOfSlots; //Number of slots
    uint16_t MaxNrb; //Maximum number of resource blocks in the carrier
    uint16_t NrbPerPacket; //Number of resource blocks per packet
    uint16_t MaxORANPacketSize; //Maximum ORAN packet size
    uint16_t numOfFrames; //Number of frames


    /*functions*/
    void printVariables(); //Print variables for debugging







};


#endif // ORAN_HPP