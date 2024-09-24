#include <cstdint>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath> 
#include "ORAN.hpp"
#include <sstream>
#include <iomanip>
#include <string>
/*
*SCS to number of slots map definition
*/
const std::map<uint32_t, uint16_t> ORAN::SCS2SlotsMap = 
{
    {15, 1},
    {30, 2},
    {60, 4},
    {120, 8},
    {240, 16},
    {480, 32},
    {640, 64}
};

/*
* @brief Constructor 
* it initializes the number of slots based on the SCS
* as it searches for the SCS in the SCS2SlotsMap
* if it finds it, it initializes the number of slots
* if it doesn't find it, it prints an error message
* @param SCS: Subcarrier spacing
*/
ORAN::ORAN(uint64_t MaxPacketSize): MaxPacketSize(MaxPacketSize)
{
    parseConfigFile();
    findSlotsPerSubFrame();
    calculate();
    //printVariables();
}

ORAN::ORAN()
{
    parseConfigFile();
    findSlotsPerSubFrame();
    calculate();
    //printVariables();
}
/*
* @brief function to find the number of slots per subframe
*/
void ORAN::findSlotsPerSubFrame()
{
    auto it = SCS2SlotsMap.find(Oran_SCS);
    if(it != SCS2SlotsMap.end())
    {
        slotsPerSubFrame = it->second;
    }
    else
    {
        std::cerr << "SCS not found in the SCS2SlotsMap" << std::endl;
    }
}

/*
* @brief Create an ORAN packet
* @return ORAN packet
*/
std::vector<uint8_t> ORAN::createORANPacket() {
    std::vector<uint8_t> packet;

    // First byte is always 0x00
    packet.push_back(firstByte);

    // frameId [0:99], needs all the second byte
    packet.push_back(frameId);  
    updateFrameId();


    // subframeId [0:9], needs 4 bits
    // slotId [0:7], needs 3 bits
    // Pack subframeId (4 bits) and slotId (3 bits) into one byte
    // subframeId occupies the 4 most significant bits and slotId occupies the 3 least significant bits
    
    uint8_t secondByte = (subframeId << 4) | slotId ;
    packet.push_back(secondByte);
    updateSlotId();
    updateSubframeId();


    // symbolId [0:13], can take 4 the whole byte as slotId is only 3 bits
    
    packet.push_back(symbolId);
    updateSymbolId();

    // sectionId byte
    packet.push_back(sectionId);

    // syminc byte
    packet.push_back(syminc);

    // startPrbu
    packet.push_back(startPrbu);

    // number of RBs used
    packet.push_back(static_cast<uint8_t>(Oran_NrbPerPacket));

    // Parse the IQ data from the file
    parseIQData(packet);

    //printPacket(packet);

    return packet;
}

/*
* @brief function to parse the IQ data from the file
* @return the parsed IQ data as a vector of bytes
* @note: the IQ data is stored in a file with the following format:
* real1 imag1
* real2 imag2
* ...
* realN imagN
* where real and imag are signed integers
* each sample is stored in 4 bytes
* real is stored in the lower 16 bits and imag is stored in the higher 16 bits
*/
void  ORAN::parseIQData(std::vector<uint8_t> &packet) {
    std::ifstream infile(Oran_Payload);
    

    if (!infile) {
        std::cerr << "Error opening file!" << std::endl;
    }

    int real, imag;
    std::string line;
    
    // To count the number of bytes pushed into the packet
    size_t bytesPushed = 0;
    
    // Each IQ sample contains 4 bytes (2 for real, 2 for imaginary)
    const size_t bytesPerSample = 4;

    while (std::getline(infile, line) && bytesPushed < BytesPerPayload) {
        std::istringstream iss(line);
        while (iss >> real >> imag && bytesPushed < BytesPerPayload) {
            // Convert real and imaginary parts to 16-bit integers
            int16_t real16 = convertTo16Bit(real);
            int16_t imag16 = convertTo16Bit(imag);

            // Break the 16-bit real part into 2 bytes and add to the vector
            packet.push_back(static_cast<uint8_t>(real16 & 0xFF)); // lower byte
            packet.push_back(static_cast<uint8_t>((real16 >> 8) & 0xFF)); // higher byte
            
            // Break the 16-bit imaginary part into 2 bytes and add to the vector
            packet.push_back(static_cast<uint8_t>(imag16 & 0xFF)); // lower byte
            packet.push_back(static_cast<uint8_t>((imag16 >> 8) & 0xFF)); // higher byte
            
            // Update the number of bytes pushed
            bytesPushed += bytesPerSample;
        }
    }

    infile.close();
}


/*
* @brief function to convert an integer to a 16-bit integer
* @param value: the integer to be converted
* @return the 16-bit integer
*/
int16_t ORAN::convertTo16Bit(int value) {

    return static_cast<int16_t>(value);
}


void ORAN::printPacket(const std::vector<uint8_t>& packet) {
    const size_t bytesPerLine = 4;  // Number of bytes per line
    size_t lineNumber = 0;  // Start line numbering from 0

    std::cout << "ORAN packet:" << std::endl;
    
    // Loop through the packet and print 4 bytes per line
    for (size_t i = 0; i < packet.size(); i += bytesPerLine) {
        // Print line number
        std::cout << "Line " << lineNumber++ << ": ";

        // Print the next 4 bytes in hexadecimal format
        for (size_t j = 0; j < bytesPerLine && (i + j) < packet.size(); ++j) {
            // Print each byte in two-digit hex format
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(packet[i + j]) << " ";
        }

        std::cout << std::dec << std::endl;  // Switch back to decimal for line numbering
    }
}


/**************************************************** ID updae functions ****************************************************/
void ORAN::updateFrameId() {
    static uint8_t packetsPerFrameCounter = 0;
    packetsPerFrameCounter++;
    if (packetsPerFrameCounter% packetsPerFrame == 0) {
        frameId++;
    }
    if (frameId == maxFrameId+1) {
        frameId = 0;
    }
}

void ORAN::updateSubframeId() {
    static uint8_t packetsPerSubFrameCounter = 0;
    packetsPerSubFrameCounter++;
    if (packetsPerSubFrameCounter % packetsPerSubFrame == 0) {
        subframeId++;
    }
    if (subframeId == maxSubframeId+1) {
        subframeId = 0;
    }
}

void ORAN::updateSlotId() {
    static uint8_t packetsPerSlotCounter = 0;
    packetsPerSlotCounter++;
    if (packetsPerSlotCounter % packetsPerSlot == 0) {
        slotId++;
    }
    if (slotId == maxSlotId+1) {
        slotId = 0;
    }
}

void ORAN::updateSymbolId() {
    static uint8_t packetsPerSymbolCounter = 0;
    packetsPerSymbolCounter++;
    if (packetsPerSymbolCounter % packetsPerSymbol == 0) {
        symbolId++;
    }
    if (symbolId == maxSymbolId+1) {
        symbolId = 0;
    }
}





/*
@prief: function to parse the configuration file
*/
void ORAN::parseConfigFile()
{
    // Open the configuration file
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Error opening file: " << configFilePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        // Skip comments
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos); // Remove comment
        }
        
        // Trim whitespace
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());

        // Parse the line
        if (line.find("Eth.LineRate") == 0) {
            LineRate = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Eth.CaptureSizeMs") == 0) {
            CaptureSizeMs = std::stoul(line.substr(line.find('=') + 1));
        }
        else if (line.find("Oran.SCS") == 0) {
            Oran_SCS = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Oran.MaxNrb") == 0) {
            Oran_MaxNrb = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Oran.NrbPerPacket") == 0) {
            Oran_NrbPerPacket = std::stoul(line.substr(line.find('=') + 1));
        } else if (line.find("Oran.PayloadType") == 0) {
            Oran_PayloadType = line.substr(line.find('=') + 1);
        } else if (line.find("Oran.Payload") == 0) {
            Oran_Payload = line.substr(line.find('=') + 1);
        }
 
    }

    configFile.close();
}

/**************************************************** calculations functions ****************************************************/
/*
@prief: function to update the frame ID
*/
void ORAN::calculate()
{
    calceBytesPerPacket();
    calcBytesPerPayload();
    calcPacketsPerSymbol();
    calcPacketsPerSlot();
    calcPacketsPerSubFrame();
    calcPacketsPerFrame();
    calcPacketsPerSec();
    calcTotalNumOfOranPackets();
    calcNumOfFrames();
    calcTotalNumOfBytes();
}

/*
@prief: function to print variables for debugging
*/
void ORAN::calceBytesPerPacket()
{
    BytesPerPacket = Oran_NrbPerPacket * BytesPerRb;
}

/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcBytesPerPayload()
{
    BytesPerPayload = BytesPerPacket - ORANPacketHeaderBytes;
}

/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcPacketsPerSymbol()
{
    packetsPerSymbol = std::ceil( static_cast<double>(Oran_MaxNrb) / static_cast<double>(Oran_NrbPerPacket));
}

/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcPacketsPerSlot()
{
    packetsPerSlot = packetsPerSymbol * symbolPerSlot;
}
/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcPacketsPerSubFrame()
{
    packetsPerSubFrame = packetsPerSlot * slotsPerSubFrame;
}

/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcPacketsPerFrame()
{
    packetsPerFrame = packetsPerSubFrame * subFramePerFrame;
}

/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcPacketsPerSec()
{
    packetsPerSec = packetsPerFrame * framesPerSec;
}


/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcTotalNumOfOranPackets()
{
    totalNumOfOranPackets = packetsPerSec * CaptureSizeMs / 1000; //capture size in Ms
    std::cout << "Total number of ORAN packets: " << totalNumOfOranPackets << std::endl;
}

/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcNumOfFrames()
{
    framesPerCapture = CaptureSizeMs / frameTimeMs;
}

/*
@prief: function to calculate the total number of bytes transmitted
*/
void ORAN::calcTotalNumOfBytes()
{
    totalNumOfBytes = totalNumOfOranPackets * BytesPerPacket;
}



void ORAN::printVariables()
{
    std::cout<<"\n";
    std::cout <<"******************************************************************************************\n";
    std::cout <<"************************************ ORAN config  Variables ***************************************\n";
    std::cout <<"Line Rate: " << LineRate << " Gbps\n";
    std::cout <<"Capture Size: " << CaptureSizeMs << " ms\n";
    std::cout <<"Frames Per Capture: " << framesPerCapture << "\n";
    std::cout <<"Max Packet Size: " << MaxPacketSize << " bytes\n";
    std::cout <<"Oran SCS: " << Oran_SCS << "\n";
    std::cout <<"Oran Max Nrb: " << Oran_MaxNrb << "\n";
    std::cout <<"Oran Nrb Per Packet: " << Oran_NrbPerPacket << "\n";   
    std::cout <<"Oran Payload Type: " << Oran_PayloadType << "\n";
    std::cout <<"Oran Payload: " << Oran_Payload << "\n";
    std::cout <<"Slots Per Sub Frame: " << slotsPerSubFrame << "\n";
    std::cout <<"Bytes Per Packet: " << BytesPerPacket << "\n";
    std::cout <<"Bytes Per Payload: " << BytesPerPayload << "\n";
    std::cout <<"Packets Per Symbol: " << packetsPerSymbol << "\n";
    std::cout <<"Packets Per Slot: " << packetsPerSlot << "\n";
    std::cout <<"Packets Per Sub Frame: " << packetsPerSubFrame << "\n";
    std::cout <<"Packets Per Frame: " << packetsPerFrame << "\n";
    std::cout <<"Packets Per Sec: " << packetsPerSec << "\n";
    std::cout <<"Total Num Of Oran Packets: " << totalNumOfOranPackets << "\n";
    std::cout <<"Total Num Of Bytes: " << totalNumOfBytes << "\n";

}

/**************************************************** getters ****************************************************/
uint32_t ORAN::getTotalNumOfPackets() const
{
    return totalNumOfOranPackets;
}