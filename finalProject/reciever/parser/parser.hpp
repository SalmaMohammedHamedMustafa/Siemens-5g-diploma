#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstdint>
#include <string>
#include <map>
#include <sys/types.h>
#include "../ORAN/ORAN.hpp"
#include "../CRC/CRC.hpp"

class Parser {
    public:
        /*
        * @brief Constructor
        */
        Parser();  

        /*
        * @brief Parse the configuration file raw data into the output file
        * @param inputFilePath Path to the input file
        * @param outputFilePath Path to the output file
        */
        void parse(const std::string& inputFilePath, const std::string& outputFilePath);
        /*
        * @brief Check the CRC of the input file and write the results to the output file
        * @param inputFilePath Path to the input file
        * @param outputFilePath Path to the output file
        */
        void CRCCheck(const std::string& inputFilePath,const std::string& outputFilePath);

    private:
        static constexpr uint16_t ifgSize = 12; // Inter-frame gap in bytes
        ORAN oran; //ORAN instance
        CRC crc; //CRC instance
        std::string configFilePath = "../parser/parserConfig.txt"; //Path to the configuration file
        uint8_t IFG = 0x07;
        int EthernetMaxPacketSize; //Ethernet maximum packet size
        int EthernetHeaderBytes; //Ethernet header bytes
        int EthernetCRCBytes; //Ethernet CRC bytes
        int eCPRIHeadersBytes; //eCPRI headers bytes
        int OranHeadersBytes; //ORAN headers bytes
        std::map<std::string, int> RecievedValuesMap; //Map to store the values read from the parsed file and their corresponding keys

        long int PacketSize; //Packet size
        long int numOfPackets; //Number of packets

        void parseConfigFile(); //Parse the configuration file
        void printMembers(); //Print the class members
        void calculatePacketSize(); //Calculate the packet size
        void calculateNumOfPackets(); //Calculate the number of packets
        void parsePacketData(const std::string& inputFilePath, const std::string& outputFilePath);
        void processPacketCRC(const std::vector<uint8_t>& packet, size_t packetNumber); //check the CRC of the packet

};
#endif // PARSER_HPP