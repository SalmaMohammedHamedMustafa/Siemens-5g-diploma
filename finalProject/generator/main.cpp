#include "Ethernet/Ethernet.hpp"
#include "socket_handler/socket_handler.hpp"
#include <iostream>

void sendFile();

int main() {
    // Create an Ethernet object and generate data
    Ethernet eth;
    eth.generate();
    int choice;
    std::cout << "Enter 1 to send the file: ";
    std::cin >> choice;
    if (choice == 1)
    {
        sendFile();
    }
    return 0;
}








void sendFile() {
    // Create a TCP server socket on port 8080
    TcpServerSocket server(8080);

    // Bind the server socket and start listening for connections
    server.BindSocket();
    server.ListenForConnections(1);  // 1 indicates the maximum number of clients in the queue

    // Wait for the client to connect
    std::cout << "Waiting for a client to connect..." << std::endl;

    int clientSocket = server.AcceptConnection();
    
    // Check if the connection is successful
    if (clientSocket > 0) {
        std::cout << "Client connected! Sending file..." << std::endl;
        
        // Send the file data to the connected client
        server.SendFileData("../output.txt");
        
        std::cout << "File sent successfully!" << std::endl;
    }
}