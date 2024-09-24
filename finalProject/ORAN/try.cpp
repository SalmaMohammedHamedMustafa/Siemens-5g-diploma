#include "ORAN.hpp"

int main()
{
    ORAN oran(100);
    std::vector<uint8_t> oranPacket = oran.createORANPacket();
    
    return 0;
}