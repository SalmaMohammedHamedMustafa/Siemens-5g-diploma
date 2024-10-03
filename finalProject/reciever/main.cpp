#include "socket_handler/socket_handler.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <string>

int main() {
    // Variables for IP address and port of the server
    std::string serverIp;  // Use std::string for easier input handling
    int serverPort;

    // Prompt user for IP address and port
    std::cout << "Enter server IP address: ";
    std::cin >> serverIp;  // Read IP address from user input
    std::cout << "Enter server port: ";
    std::cin >> serverPort;  // Read port number from user input

    const char* filePath = "../received_file.txt";  // Path where the received file will be saved

    try {
        // Initialize client socket with server IP and port
        TcpClientSocket clientSocket(const_cast<char*>(serverIp.c_str()));  // Convert const char* to char*

        // Connect to the server
        clientSocket.ConnectToServer();
        std::cout << "Connected to server at " << serverIp << ":" << serverPort << std::endl;

        // Start receiving file from the server
        std::cout << "Receiving file from server..." << std::endl;
        clientSocket.ReceiveFileData(filePath);

        std::cout << "File received and saved to " << filePath << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    Parser parser;
    std::cout << "Parsing the received file..." << std::endl;
    parser.parse(filePath, "../raw_data.txt");
    std::cout << "Parsing complete. Results saved to raw_data.txt" << std::endl;
    std::cout << "Checking CRC for each packet..." << std::endl;
    parser.CRCCheck(filePath, "../CRC_results.txt");
    std::cout << "CRC check complete. Results saved to CRC_results.txt" << std::endl;

    return 0;
}