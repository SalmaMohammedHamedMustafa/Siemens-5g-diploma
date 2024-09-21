#include "../ECPRI/ECPRI.hpp"
#include <cstdint>
#include <sys/types.h>

int main()
{
    uint32_t EthernetHeaderBytes = 26;
    uint32_t EthernetPacketBytes = 1500;
    uint32_t MaxORANPacketSize = EthernetPacketBytes - EthernetHeaderBytes ;  
    ECPRI ecpri(30, 273, 30, (1500-26));
    std::vector<uint8_t> ecpriPacket = ecpri.createECPRIPacket();
    
    return 0;
}