#include "CRC.hpp"
#include <iostream>

CRC::CRC() {
    generateCRCTable();
}
void CRC::generateCRCTable() {
    // Generate the CRC table using the polynomial
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
        crcTable[i] = crc;
    }
}

uint32_t CRC::calculateCRC32(const std::vector<uint8_t>& packet) {
    uint32_t crc = initialCrcValue;

    // Iterate over the packet bytes (excluding CRC at the end, if any)
    for (size_t i = 0; i < packet.size(); i++) {
        uint8_t byte = packet[i];
        crc = (crc >> 8) ^ crcTable[(crc ^ byte) & 0xFF];
    }

    return ~crc;  // Final XOR to get the CRC
}

void CRC::applyCRC(std::vector<uint8_t>& packet) {
    // Calculate CRC-32 for the packet (without CRC bytes)
    uint32_t crc = calculateCRC32(packet);

    // Append the CRC to the packet (4 bytes)
    for (int i = 0; i < 4; ++i) {
        packet.push_back(static_cast<uint8_t>((crc >> (8 * i)) & 0xFF));
    }
}

bool CRC::verifyCRC(const std::vector<uint8_t>& packet) {
    if (packet.size() < 4) {
        std::cerr << "Packet too short to contain a valid CRC!" << std::endl;
        return false;
    }

    // Extract the CRC from the last 4 bytes of the packet
    uint32_t extractedCRC = 0;
    for (int i = 0; i < 4; ++i) {
        extractedCRC |= (static_cast<uint32_t>(packet[packet.size() - 4 + i]) << (8 * i));
    }

    // Calculate CRC over the entire packet (excluding the last 4 bytes, which contain the CRC)
    uint32_t calculatedCRC = calculateCRC32(std::vector<uint8_t>(packet.begin(), packet.end() - 4));

    // Compare calculated CRC with extracted CRC
    return extractedCRC == calculatedCRC;
}
