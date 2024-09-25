#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <map>
#include "ORAN.hpp"

class Parser {
    public:
        /*
        * @brief Constructor
        */
        Parser();  

        /*
        * @brief Parse the configuration file
        */
        void parse(std::string FilePath);

    private:
        ORAN oran; //ORAN instance
        std::string configFilePath = "parserConfig.txt"; //Path to the configuration file
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


};
#endif // PARSER_HPP