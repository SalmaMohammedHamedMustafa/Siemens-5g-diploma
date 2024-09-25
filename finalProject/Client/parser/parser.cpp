#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>


// Constructor implementation
Parser::Parser() {
    parseConfigFile();
    oran.setMaxPacketSize(EthernetMaxPacketSize - EthernetHeaderBytes - EthernetCRCBytes - eCPRIHeadersBytes);
    numOfPackets = oran.getTotalNumOfPackets();
    PacketSize = oran.getPacketSize();
    printMembers();
}

void Parser::parseConfigFile() {
    std::ifstream file(configFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open configuration file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        char equalSign;
        int value;

        // Parse the key-value pair (e.g., EthernetMax = 1500)
        if (iss >> key >> equalSign >> value) {
            if (equalSign == '=') {
                RecievedValuesMap[key] = value;

                // Store parsed values into class members if applicable
                if (key == "EthernetMax") {
                    EthernetMaxPacketSize = value;
                } else if (key == "EthernetHeaders") {
                    EthernetHeaderBytes = value;
                } else if (key == "EthernetCRC") {
                    EthernetCRCBytes = value;
                } else if (key == "eCPRIHeaders") {
                    eCPRIHeadersBytes = value;
                } else if (key == "ORANHeaders") {
                    OranHeadersBytes = value;
                }
            }
        } else {
            // Handle received hexadecimal values like a5 ff = -91
            std::string hexValue;
            int parsedValue;
            if (iss >> hexValue >> equalSign >> parsedValue) {
                if (equalSign == '=') {
                    // Parse hexadecimal numbers
                    std::istringstream hexStream(hexValue);
                    int hexNumber;
                    hexStream >> std::hex >> hexNumber;

                    RecievedValuesMap[hexValue] = parsedValue;
                }
            }
        }
    }

    file.close();
}

void Parser::printMembers() {
    std::cout << "EthernetMaxPacketSize: " << EthernetMaxPacketSize << std::endl;
    std::cout << "EthernetHeaderBytes: " << EthernetHeaderBytes << std::endl;
    std::cout << "EthernetCRCBytes: " << EthernetCRCBytes << std::endl;
    std::cout << "eCPRIHeadersBytes: " << eCPRIHeadersBytes << std::endl;
    std::cout << "OranHeadersBytes: " << OranHeadersBytes << std::endl;
    std::cout << "PacketSize: " << PacketSize << std::endl;
    std::cout << "numOfPackets: " << numOfPackets << std::endl;

}