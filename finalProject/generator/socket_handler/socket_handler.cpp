#include <cstdlib>
#include <iostream>
#include <fstream> // For file handling
#include <cstring> // For strlen
#include "socket_handler.hpp"



/**
 * @brief Constructs a TcpSocket object and initializes the socket.
 * @param WantedServerIp A pointer to a character array representing the server's IP address.
 * 
 * This constructor creates a socket and sets up the server address structure.
 */
TcpClientSocket::TcpClientSocket(char* WantedServerIp) {
    try {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0) {
            throw std::runtime_error("Error creating socket!");
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    ServerIp = WantedServerIp;
    serverAddr.sin_family = AF_INET;          // IPv4
    serverAddr.sin_port = htons(8080);        // Port number to connect to
    serverAddr.sin_addr.s_addr = inet_addr(ServerIp); // Server IP address
}

/**
 * @brief Connects the client socket to the server.
 * 
 * Attempts to establish a connection to the server using the provided server address.
 * If the connection fails, an exception is thrown, and the socket is closed.
 * 
 * @throw std::runtime_error If the connection fails.
 */
void TcpClientSocket::ConnectToServer() {
    try {
        int connectionStatus = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (connectionStatus < 0) {
            throw std::runtime_error("Error connecting to server!");
        }

        std::cout << "Connected to server successfully!" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        close(clientSocket); // Close the socket on error
        std::exit(1);
    }
}

/**
 * @brief Sends data to the connected server.
 * 
 * @param message A pointer to a character array containing the message to be sent.
 */
void TcpClientSocket::SendData(char* message) {
    send(clientSocket, message, strlen(message), 0);
}

/**
 * @brief Receives data from the connected server.
 * 
 * This function reads data from the server and stores it in the buffer.
 */
void TcpClientSocket::ReceiveData() {
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[bytesReceived] = '\0'; // Null-terminate the received data
}

/**
 * @brief Retrieves the buffer containing received data.
 * 
 * @return const char* A pointer to the buffer containing data received from the server.
 */
char* TcpClientSocket::GetBuffer() {
    return buffer;
}
/*
    * @brief Checks if the connection is still alive.
    * 
    * This function checks if the connection is still alive by peeking at the incoming data.
    * If the connection is still alive, the function returns true; otherwise, it returns false.
    * 
    * @return bool True if the connection is alive, false otherwise.
*/

bool TcpClientSocket::isConnectionAlive() {
    char buffer;
    ssize_t result = recv(clientSocket, &buffer, 1, MSG_PEEK);

    if (result > 0) {
        // Data is available to read, connection is alive
        return true;
    } else if (result == 0) {
        // Connection has been closed by the peer
        return false;
    } else {
        // Error occurred
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // No data to read right now, but the connection is still alive
            return true;
        } else {
            // Some other error occurred, consider the connection as closed
            return false;
        }
    }
}

void TcpClientSocket::ReceiveFileData(const char* filePath) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return;
    }

    // Increase receive buffer size
    int bufferSize = 65536;  // 64KB
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));

    char buffer[8192];  // Larger buffer size
    ssize_t bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        if (bytesReceived < 0) {
            std::cerr << "Error receiving data from server!" << std::endl;
            file.close();
            return;
        }

        file.write(buffer, bytesReceived);

        // Send acknowledgment after writing data to the file
        const char ack = 'A';
        ssize_t ackSent = send(clientSocket, &ack, sizeof(ack), 0);
        if (ackSent < 0) {
            std::cerr << "Error sending acknowledgment to server!" << std::endl;
            file.close();
            return;
        }
    }

    if (bytesReceived == 0) {
        std::cout << "File transfer complete!" << std::endl;
    } else if (bytesReceived < 0) {
        std::cerr << "Error receiving file data!" << std::endl;
    }

    file.close();
}

void TcpClientSocket::SendFileData(const char* filePath) {
    FILE* file = fopen(filePath, "rb");
    if (file == nullptr) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    // Increase send buffer size
    int bufferSize = 65536;  // 64KB
    setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));

    char fileBuffer[8192];  // Larger buffer size
    size_t bytesRead;
    ssize_t totalBytesSent = 0;

    while ((bytesRead = fread(fileBuffer, 1, sizeof(fileBuffer), file)) > 0) {
        size_t bytesLeftToSend = bytesRead;
        const char* bufferPtr = fileBuffer;

        while (bytesLeftToSend > 0) {
            ssize_t bytesSent = send(clientSocket, bufferPtr, bytesLeftToSend, 0);
            if (bytesSent < 0) {
                std::cerr << "Error sending file data to client!" << std::endl;
                fclose(file);
                return;
            }

            bufferPtr += bytesSent;
            bytesLeftToSend -= bytesSent;
            totalBytesSent += bytesSent;
        }

        // Wait for acknowledgment from the client
        char ackBuffer[2];
        ssize_t ackReceived = recv(clientSocket, ackBuffer, sizeof(ackBuffer), 0);
        if (ackReceived <= 0 || ackBuffer[0] != 'A') {
            std::cerr << "Error receiving acknowledgment from client!" << std::endl;
            fclose(file);
            return;
        }
    }

    if (feof(file)) {
        // Print only the final total number of bytes sent
        std::cout << "File transfer complete! Total bytes sent: " << totalBytesSent << " bytes." << std::endl;
    } else {
        std::cerr << "Error reading file!" << std::endl;
    }

    fclose(file);
}

/**
 * @brief Destructor for the TcpSocket class.
 * 
 * Closes the client socket when the TcpSocket object is destroyed.
 */
TcpClientSocket::~TcpClientSocket() {
    close(clientSocket);
}



// TcpServerSocket method implementations

TcpServerSocket::TcpServerSocket(int port) {
    try {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            throw std::runtime_error("Error creating server socket!");
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
}

void TcpServerSocket::BindSocket() {
    try {
        int bindStatus = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (bindStatus < 0) {
            throw std::runtime_error("Error binding server socket!");
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        close(serverSocket);
        std::exit(1);
    }
}

void TcpServerSocket::ListenForConnections(int backlog) {
    try {
        int listenStatus = listen(serverSocket, backlog);
        if (listenStatus < 0) {
            throw std::runtime_error("Error listening on server socket!");
        }
        std::cout << "Server is listening for connections..." << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        close(serverSocket);
    }
}

int TcpServerSocket::AcceptConnection() {
    socklen_t addrLen = sizeof(clientAddr);
    try {
        clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);
        if (clientSocket < 0) {
            throw std::runtime_error("Error accepting client connection!");
        }
        std::cout << "Client connected successfully!" << std::endl;
        return clientSocket;
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}

void TcpServerSocket::SendData(char* message) {
    send(clientSocket, message, strlen(message), 0);
}

void TcpServerSocket::ReceiveData() {
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[bytesReceived] = '\0'; // Null-terminate the received data
}

char* TcpServerSocket::GetBuffer() {
    return buffer;
}

TcpServerSocket::~TcpServerSocket() {
    shutdown(serverSocket, SHUT_RDWR);
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    close(serverSocket);
}


bool TcpServerSocket::isConnectionAlive(int socket) {
    char buffer;
    ssize_t result = recv(socket, &buffer, 1, MSG_PEEK);

    if (result > 0) {
        // Data is available to read, connection is alive
        return true;
    } else if (result == 0) {
        // Connection has been closed by the peer
        return false;
    } else {
        // Error occurred
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // No data to read right now, but the connection is still alive
            return true;
        } else {
            // Some other error occurred, consider the connection as closed
            return false;
        }
    }
}

void TcpServerSocket::SendFileData(const char* filePath) {
    FILE* file = fopen(filePath, "rb");
    if (file == nullptr) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    // Increase send buffer size
    int bufferSize = 65536;  // 64KB
    setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));

    char fileBuffer[8192];  // Larger buffer size
    size_t bytesRead;
    ssize_t totalBytesSent = 0;

    while ((bytesRead = fread(fileBuffer, 1, sizeof(fileBuffer), file)) > 0) {
        size_t bytesLeftToSend = bytesRead;
        const char* bufferPtr = fileBuffer;

        while (bytesLeftToSend > 0) {
            ssize_t bytesSent = send(clientSocket, bufferPtr, bytesLeftToSend, 0);
            if (bytesSent < 0) {
                std::cerr << "Error sending file data to client!" << std::endl;
                fclose(file);
                return;
            }

            bufferPtr += bytesSent;
            bytesLeftToSend -= bytesSent;
            totalBytesSent += bytesSent;
        }

        // Wait for acknowledgment from the client
        char ackBuffer[2];
        ssize_t ackReceived = recv(clientSocket, ackBuffer, sizeof(ackBuffer), 0);
        if (ackReceived <= 0 || ackBuffer[0] != 'A') {
            std::cerr << "Error receiving acknowledgment from client!" << std::endl;
            fclose(file);
            return;
        }
    }

    if (feof(file)) {
        // Print only the final total number of bytes sent
        std::cout << "File transfer complete! Total bytes sent: " << totalBytesSent << " bytes." << std::endl;
    } else {
        std::cerr << "Error reading file!" << std::endl;
    }

    fclose(file);
}

void TcpServerSocket::ReceiveFileData(const char* filePath) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return;
    }

    // Increase receive buffer size
    int bufferSize = 65536;  // 64KB
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));

    char buffer[8192];  // Larger buffer size
    ssize_t bytesReceived;

    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        if (bytesReceived < 0) {
            std::cerr << "Error receiving data from server!" << std::endl;
            file.close();
            return;
        }

        file.write(buffer, bytesReceived);

        // Send acknowledgment after writing data to the file
        const char ack = 'A';
        ssize_t ackSent = send(clientSocket, &ack, sizeof(ack), 0);
        if (ackSent < 0) {
            std::cerr << "Error sending acknowledgment to server!" << std::endl;
            file.close();
            return;
        }
    }

    if (bytesReceived == 0) {
        std::cout << "File transfer complete!" << std::endl;
    } else if (bytesReceived < 0) {
        std::cerr << "Error receiving file data!" << std::endl;
    }

    file.close();
}