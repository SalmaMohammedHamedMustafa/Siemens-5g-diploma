#ifndef CRC_HPP
#define CRC_HPP
#include <vector>
#include <cstdint>


class CRC
{
public:
    CRC();

    // Function to generate CRC for a packet and append it
    void applyCRC(std::vector<uint8_t>& packet);

    // Function to verify the CRC at the receiver side
    bool verifyCRC(const std::vector<uint8_t>& packet);

    // Function to generate the CRC lookup table
    void generateCRCTable();

    // Function to calculate CRC-32 for a given packet (excluding the last 4 CRC bytes if present)
    uint32_t calculateCRC32(const std::vector<uint8_t>& packet);

    // Utility function to print the contents of a packet
    void printPacket(const std::vector<uint8_t>& packet);

private:
    uint32_t crcTable[256];  // CRC lookup table
    static constexpr uint32_t polynomial = 0xEDB88320;  // Polynomial used for CRC-32
    static constexpr uint32_t initialCrcValue = 0xFFFFFFFF; // Initial value for CRC calculation
};

#endif // CRC_HPP