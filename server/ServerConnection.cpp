#include "ServerConnection.h"

ServerConnection::ServerConnection() {}
ServerConnection::ServerConnection(sockpp::tcp_socket &socket) {
    _socket = sockpp::tcp_socket(socket.handle());
    _socketID = socket.handle();
}

void ServerConnection::sendData(void *data, uint32_t size) {
    _socket.write_n(data, size);
}