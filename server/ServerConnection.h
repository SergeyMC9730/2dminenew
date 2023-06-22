#pragma once

#include <sockpp/tcp_socket.h>

class ServerConnection {
public:
    ServerConnection(sockpp::tcp_socket &socket);
    ServerConnection();

    sockpp::tcp_socket _socket;
    uint32_t _socketID;

    void sendData(void *data, uint32_t size);
};