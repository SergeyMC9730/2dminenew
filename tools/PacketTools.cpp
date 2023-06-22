/*
    2DMine -- 2D minecraft on raylib
    Copyright (C) 2023  Sergei Baigerov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "PacketTools.hpp"

// PacketTools::PacketTools(sockpp::tcp_socket socket) {
//     // _socket = socket;
//     _socket = std::move(socket);
//     _callback = [](PacketReceiver *a, net::packet::MetaPacket &b) {};
//     _thread = std::thread([](sockpp::tcp_socket sock, PacketReceiver *self) {
//         char *buffer = (char *)malloc(600);
//         ssize_t n = 0;

//         while (true) {
//             n = sock.read(buffer, 600);

//             if (n <= 0) {
//                 std::cout << "SERVER  :  connection lost" << std::endl;
//                 free(buffer);
//                 return;
//             }

//             net::packet::MetaPacket packet;
//             packet.ParseFromArray(buffer, 600);

//             std::cout << "SERVER  :  Received packet " << packet.type() << std::endl;

//             bzero(buffer, 600);

//             self->receivePacket(packet);
//         }
//     }, std::move(socket), this);

//     _thread.detach();

//     // if (join) {
//     //     _thread.join();
//     // }
// }

// void PacketReceiver::joinReceiver() {
//     _thread.join();
// }

// void PacketReceiver::receivePacket(net::packet::MetaPacket &packet) {
//     _callback(this, packet);

//     return;
// }

// void PacketReceiver::sendPacket(net::packet::MetaPacket &packet, bool cleanup) {
//     size_t size = packet.ByteSizeLong();
//     void *buffer = malloc(size);

//     packet.SerializeToArray(buffer, size);
    
//     _socket.write_n(buffer, size);
    
//     free(buffer);

//     if (cleanup) {
//         packet.mutable_packet()->clear();
//     }

//     return;
// }

void PacketTools::sendPacket(net::packet::MetaPacket &packet, sockpp::tcp_connector &connector) {
    void *additional_data = malloc(1);
    
    if (packet.packet().size() == 0) {
        packet.set_packet(additional_data, 1);
    }

    size_t size = packet.ByteSizeLong();
    void *data = malloc(size);

    packet.SerializeToArray(data, size);

    connector.write_n(data, size);

    free(additional_data);
    free(data);

    // std::cout << "Sent packet " << packet.type() << "\n";
}

void PacketTools::sendPacket(net::packet::MetaPacket &packet, sockpp::tcp_socket &sock) {
    void *additional_data = malloc(1);
    
    if (packet.packet().size() == 0) {
        packet.set_packet(additional_data, 1);
    }

    size_t size = packet.ByteSizeLong();
    void *data = malloc(size);

    packet.SerializeToArray(data, size);

    sock.write_n(data, size);

    free(additional_data);
    free(data);

    // std::cout << "Sent packet " << packet.type() << "\n";
}

void PacketTools::sendEmptyPacket(uint32_t type, sockpp::tcp_socket &sock) {
    net::packet::MetaPacket mp;
    
    mp.set_type(type);

    return PacketTools::sendPacket(mp, sock);
}
void PacketTools::sendEmptyPacket(uint32_t type, sockpp::tcp_connector &conn) {
    net::packet::MetaPacket mp;
    
    mp.set_type(type);

    return PacketTools::sendPacket(mp, conn);
}

void PacketTools::sendHelloPacket(sockpp::tcp_socket &sock) {
    return sendEmptyPacket(PT_HELLO, sock);
}