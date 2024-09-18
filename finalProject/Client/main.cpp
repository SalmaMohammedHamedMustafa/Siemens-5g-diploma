#include "socket_handler/socket_handler.hpp"
#include <iostream>

int main() {
    // IP address and port of the server
    char serverIp[] = "192.168.1.5";  // Replace with the actual server IP address
    int serverPort = 8080;          // Replace with the actual port number
    const char* filePath = "received_file.txt";  // Path where the received file will be saved

    try {
        // Initialize client socket with server IP and port
        TcpClientSocket clientSocket(serverIp);

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

    return 0;
}
