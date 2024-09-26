#include "parser.hpp"
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <cmath>
#include <iomanip>


// Constructor implementation
Parser::Parser() {
    parseConfigFile();
    oran.setMaxPacketSize(EthernetMaxPacketSize - EthernetHeaderBytes - EthernetCRCBytes - eCPRIHeadersBytes);
    numOfPackets = oran.getTotalNumOfPackets();
    PacketSize = oran.getPacketSize() + EthernetHeaderBytes + EthernetCRCBytes + eCPRIHeadersBytes ;
    //printMembers();
}


void Parser::parseConfigFile() {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open configuration file. parser" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        // Trim leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        // Ignore comment lines (starting with '#') or empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string key;
        int value;



            // Assign values to the corresponding class members based on the key
            if (line.find("EthernetMax") == 0) {
            EthernetMaxPacketSize = std::stoul(line.substr(line.find('=') + 1));
            } 
            else if (line.find("EthernetHeader") == 0) {
            EthernetHeaderBytes = std::stoul(line.substr(line.find('=') + 1));
            } 
            else if (line.find("EthernetCRC") == 0) {
            EthernetCRCBytes = std::stoul(line.substr(line.find('=') + 1));
            } 
            else if (line.find("eCPRIHeaders") == 0) {
            eCPRIHeadersBytes = std::stoul(line.substr(line.find('=') + 1));
            } 
            else if (line.find("ORANHeaders") == 0) {
            OranHeadersBytes = std::stoul(line.substr(line.find('=') + 1));
            }

            else {
                // Check if the line contains a key-value pair
            if (std::getline(ss, key, ':')) {
            // Remove any trailing whitespace from the key
            key.erase(key.find_last_not_of(" \t\n\r") + 1);
            // Read the value
            if (!(ss >> value)) {
                std::cerr << "Error: Invalid value format for key: " << key << std::endl;
                continue;
            }
                std::stringstream keyStream(key);
                unsigned int hexKey1, hexKey2;
                char separator;

                // Parse hexadecimal keys (e.g., "a5 ff")
                if (keyStream >> std::hex >> hexKey1 >> separator >> std::hex >> hexKey2) {
                    std::string hexString = key;
                    RecievedValuesMap[hexString] = value;
                }
            }
        }
    }

    configFile.close();
}

/*****************************************************************Parser ************************************************/
std::pair<uint8_t, uint8_t> convertToBytePair(const std::string& hexStr) {
    std::istringstream iss(hexStr);
    std::string byteStr1, byteStr2;

    // Extract the two hex bytes
    iss >> byteStr1 >> byteStr2;

    // Convert hex string to uint8_t
    uint8_t byte1 = static_cast<uint8_t>(std::stoi(byteStr1, nullptr, 16));
    uint8_t byte2 = static_cast<uint8_t>(std::stoi(byteStr2, nullptr, 16));

    return {byte1, byte2};
}

// Function to convert map<std::string, int> to map<std::pair<uint8_t, uint8_t>, int>
std::map<std::pair<uint8_t, uint8_t>, int> convertMap(const std::map<std::string, int>& receivedMap) {
    std::map<std::pair<uint8_t, uint8_t>, int> convertedMap;

    for (const auto& entry : receivedMap) {
        // Convert the string key to a pair of bytes
        std::pair<uint8_t, uint8_t> bytePair = convertToBytePair(entry.first);

        // Insert the converted pair and associated value into the new map
        convertedMap[bytePair] = entry.second;
    }

    return convertedMap;
}

void readInputFile(const std::string& inputFilePath, std::vector<std::pair<uint8_t, uint8_t>>& fileData) {
    std::ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string byteStr1, byteStr2;
        while (iss >> byteStr1 >> byteStr2) {
            uint8_t byte1 = static_cast<uint8_t>(std::stoi(byteStr1, nullptr, 16));
            uint8_t byte2 = static_cast<uint8_t>(std::stoi(byteStr2, nullptr, 16));
            fileData.emplace_back(byte1, byte2);
        }
    }
    inputFile.close();
}

void writeOutputFile(const std::string& outputFilePath, const std::vector<int>& results) {
    std::ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open output file!" << std::endl;
        return;
    }

    for (size_t i = 0; i < results.size(); ++i) {
        outputFile << std::setw(4) << results[i] << " ";
        if ((i + 1) % 2 == 0) {
            outputFile << std::endl;
        }
    }
    outputFile.close();
}

std::vector<int> processPacketData(const std::vector<std::pair<uint8_t, uint8_t>>& fileData, 
                                   const std::map<std::pair<uint8_t, uint8_t>, int>& valueMap, 
                                   size_t startIndex, size_t endIndex) {
    std::vector<int> result;
    for (size_t i = startIndex; i < endIndex; ++i) {
        auto key = fileData[i];
        if (valueMap.find(key) != valueMap.end()) {
            result.push_back(valueMap.at(key));
        } 
    }
    return result;
}

void Parser::parsePacketData(const std::string& inputFilePath, const std::string& outputFilePath) {
    // Read the input file
    std::vector<std::pair<uint8_t, uint8_t>> fileData;
    readInputFile(inputFilePath, fileData);

    // Define the byte mappings for 2-byte sequences
    std::map<std::pair<uint8_t, uint8_t>, int> valueMap = convertMap(RecievedValuesMap);

    // Define constants
    const size_t headerSize = EthernetHeaderBytes + eCPRIHeadersBytes + OranHeadersBytes;
    const size_t crcSize = EthernetCRCBytes;
    //padding for 4 byte alignment
    const size_t paddedSize = 4 - (PacketSize % 4);
    std::vector<int> allResults;
    for (size_t packetIndex = 0; packetIndex < numOfPackets; ++packetIndex) {
        size_t startIndex = (headerSize / 2) + packetIndex * (PacketSize / 2 + ifgSize / 2 + paddedSize / 2);
        size_t endIndex = startIndex + (PacketSize / 2);
        
        if (endIndex >= fileData.size() - crcSize / 2 && packetIndex < numOfPackets - 1) {
            std::cerr << "Error: File size is too small to process packet " << packetIndex << std::endl;
            return;
        }
        
        auto result = processPacketData(fileData, valueMap, startIndex, endIndex);
        allResults.insert(allResults.end(), result.begin(), result.end());
    }

    // Write the processed data to output file
    writeOutputFile(outputFilePath, allResults);
}

void Parser::parse(const std::string& inputFilePath, const std::string& outputFilePath) {
    parsePacketData(inputFilePath, outputFilePath);
}



/*****************************************************************CRC ************************************************/
void Parser::processPacketCRC(const std::vector<uint8_t>& packet, size_t packetNumber) {
    bool crcValid = crc.verifyCRC(packet);

    // Write the result to a file
    std::ofstream resultFile("../CRC_results.txt", std::ios::app);  // Open in append mode
    if (!resultFile.is_open()) {
        std::cerr << "Error: Could not open result file!" << std::endl;
        return;
    }

    resultFile << "Packet " << packetNumber << ": ";
    if (crcValid) {
        resultFile << "CRC is valid." << std::endl;
    } else {
        resultFile << "CRC is invalid." << std::endl;
    }

    resultFile.close();
}



void Parser::CRCCheck(const std::string& inputFilePath,const std::string& outputFilePath) {
    // Clear the result file before starting
    std::ofstream resultFile(outputFilePath, std::ios::trunc);
    if (!resultFile.is_open()) {
        std::cerr << "Error: Could not open result file!" << std::endl;
        return;
    }
    resultFile.close();  // Close it immediately after truncating

    std::ifstream file(inputFilePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    std::string line;
    size_t packetCount = 0;
    std::vector<uint8_t> currentPacket;

    while (packetCount < numOfPackets) {
        // Clear the current packet for a new read
        currentPacket.clear();

        // Continue reading until we fill a packet
        while (currentPacket.size() < PacketSize && std::getline(file, line)) {
            std::istringstream iss(line);
            std::string byteString;

            while (iss >> byteString) {
                // Convert hex string to uint8_t
                unsigned int byteValue;
                std::stringstream ss;
                ss << std::hex << byteString;
                ss >> byteValue;
                currentPacket.push_back(static_cast<uint8_t>(byteValue));

                // Break if we reach the packet size
                if (currentPacket.size() == PacketSize) {
                    break;
                }
            }
        }



        // Check if we have a complete packet
        if (currentPacket.size() == PacketSize) {
            processPacketCRC(currentPacket, packetCount);
            packetCount++;
            // Skip IFG bytes after processing the packet
            size_t bytesSkipped = 0;

            while (bytesSkipped < ifgSize) {
                if (std::getline(file, line)) {
                    // Count the number of bytes discarded
                    std::istringstream iss(line);
                    std::string byteString;

                    // Continue reading until we've skipped the required number of bytes
                    while (iss >> byteString && bytesSkipped < ifgSize) {
                        bytesSkipped++;
                    }
                } else {
                    // Handle end of file 
                    std::cerr << "End of file" << std::endl;
                    return;
                }
        }
        } else if (packetCount < numOfPackets - 1) {   
            // Handle the case where we don't have enough data for a complete packet
            std::cerr << "Incomplete packet read. Stopping extraction." << std::endl;
            break;
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
    // Print the contents of the map
    std::cout << "RecievedValuesMap:" << std::endl;
    for (const auto& pair : RecievedValuesMap) {
        std::cout << pair.first << " = " << pair.second << std::endl;
    }

}

