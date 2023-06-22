#pragma once

#include <sockpp/tcp_connector.h>

#include <cstdint>
#include <string>

namespace net {
    extern sockpp::tcp_connector connector;
    extern bool connected;

    bool connect(std::string host, uint16_t port);
    void network_thread(std::string host, uint16_t port);
}