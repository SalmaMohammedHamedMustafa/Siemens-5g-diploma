#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <cstring>      // For memset()
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h>  // For inet_addr() and sockaddr_in
#include <unistd.h>     // For close()

/**
 * @brief Abstract base class for handling TCP socket connections.
 */
class TcpSocket {
protected:
    char buffer[1024] = {0};   ///< Buffer for receiving data

public:
    /**
     * @brief Sends data through the socket.
     * @param message A pointer to a character array containing the message to be sent.
     */
    virtual void SendData(char* message) = 0;

    /**
     * @brief Receives data from the socket.
     * This function reads data from the socket and stores it in the buffer.
     */
    virtual void ReceiveData() = 0;

    /**
     * @brief Retrieves the buffer containing received data.
     * @return char* A pointer to the buffer containing data received from the socket.
     */
    virtual char* GetBuffer() = 0;

    /*
    * @brief Receives file data 
    * @param filePath A pointer to a character array containing the path to the file to be saved.
    */
    virtual void ReceiveFileData(const char* filePath)=0;

    /*
    * @brief Sends file data
    * @param filePath A pointer to a character array containing the path to the file to be sent.
    */
    virtual void SendFileData(const char* filePath)=0;

    /**
     * @brief Virtual destructor for the TcpSocket class.
     */
    virtual ~TcpSocket() = default;
};





/**
 * @brief TcpClientSocket class for handling TCP client-side connections.
 */
class TcpClientSocket : public TcpSocket {
    int clientSocket=-1;          ///< File descriptor for the client socket
    sockaddr_in serverAddr;    ///< Structure for server address
    char* ServerIp;            ///< Pointer to the server IP address

public:
    /**
     * @brief Constructs a TcpClientSocket object and initializes the socket.
     * @param WantedServerIp A pointer to a character array representing the server's IP address.
     */
    TcpClientSocket(char* WantedServerIp);

    /**
     * @brief Connects the client socket to the server.
     * @throw std::runtime_error If the connection fails.
     */
    void ConnectToServer();

    bool isConnectionAlive();

    virtual void ReceiveFileData(const char* filePath) override;

    virtual void SendFileData(const char* filePath) override;

    /**
     * @brief Sends data to the connected server.
     * @param message A pointer to a character array containing the message to be sent.
     */
    virtual void SendData(char* message) override;

    /**
     * @brief Receives data from the connected server.
     * This function reads data from the server and stores it in the buffer.
     */
    virtual void ReceiveData() override;

    /**
     * @brief Retrieves the buffer containing received data.
     * @return char* A pointer to the buffer containing data received from the server.
     */
    virtual char* GetBuffer() override;

    /**
     * @brief Destructor for the TcpClientSocket class.
     * Closes the client socket when the TcpClientSocket object is destroyed.
     */
    ~TcpClientSocket();
};




/**
 * @brief TcpServerSocket class for handling TCP server-side connections.
 */
class TcpServerSocket : public TcpSocket {
    int serverSocket=-1;              ///< File descriptor for the server socket
    int clientSocket=-1;              ///< File descriptor for the client socket
    sockaddr_in serverAddr;        ///< Structure for storing the server's address information
    sockaddr_in clientAddr;        ///< Structure for storing the connected client's address information

public:
    /**
     * @brief Constructs a TcpServerSocket object and initializes the server socket.
     * @param port The port number on which the server will listen for incoming connections.
     */
    TcpServerSocket(int port);

    /**
     * @brief Binds the server socket to the specified port.
     * This function associates the server socket with a specific port and IP address.
     * @throw std::runtime_error If binding the server socket fails.
     */
    void BindSocket();

    /**
     * @brief Puts the server socket into listening mode, ready to accept incoming connections.
     * @param backlog The maximum length of the queue for pending connections.
     * @throw std::runtime_error If the server socket fails to listen for connections.
     */
    void ListenForConnections(int backlog);

    /**
     * @brief Accepts an incoming connection from a client.
     * @return int The file descriptor for the accepted client socket.
     * @throw std::runtime_error If accepting a client connection fails.
     */
    int AcceptConnection();

    /**
     * @brief Retrieves if the connection is alive.
     * @return the socket of the connection.
     */
    bool isConnectionAlive(int socket);


    virtual void ReceiveFileData(const char* filePath) override;

    virtual void SendFileData(const char* filePath) override;


    /**
     * @brief Sends data to the connected client.
     * @param message A pointer to a character array containing the message to be sent.
     */
    void SendData(char* message) override;

    /**
     * @brief Receives data from the connected client.
     * This function reads data from the client socket and stores it in the buffer.
     */
    void ReceiveData() override;

    /**
     * @brief Retrieves the buffer containing received data.
     * @return char* A pointer to the buffer containing data received from the client.
     */
    char* GetBuffer() override;


    /**
     * @brief Destructor for the TcpServerSocket class.
     * Closes the server and client sockets when the TcpServerSocket object is destroyed.
     */
    ~TcpServerSocket();
};
#endif // TCPSOCKET_H