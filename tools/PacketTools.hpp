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

#pragma once

#include <functional>
#include <thread>

#include <MetaPacket.pb.h>
#include <sockpp/tcp_connector.h>
#include <sockpp/socket.h>

enum PacketType {
    PT_HELLO = 0,
    PT_CLIENT_REGISTER,
    PT_OK,
    PT_BALL_INFO,
    PT_BALL_DISCONNECT
};

class PacketTools {
public:
    template<class T = google::protobuf::Message>
    static net::packet::MetaPacket constructPacket(T &msg) {
        size_t size = msg.ByteSizeLong();
        void *buffer = malloc(size);

        msg.SerializeToArray(buffer, size);

        net::packet::MetaPacket metapacket;
        metapacket.set_packet(buffer, size);

        return metapacket;
    }

    template<class T = google::protobuf::Message>
    static T receivePacket(net::packet::MetaPacket &packet) {
        size_t size = packet.packet().length();
        void *data = (void *)packet.packet().data();

        T p;
        p.ParseFromArray(data, size);

        return p;
    }

    static void sendPacket(net::packet::MetaPacket &packet, sockpp::tcp_connector &connector);
    static void sendPacket(net::packet::MetaPacket &packet, sockpp::tcp_socket &sock);

    static void sendEmptyPacket(uint32_t type, sockpp::tcp_socket &sock);
    static void sendEmptyPacket(uint32_t type, sockpp::tcp_connector &connector);

    static void sendHelloPacket(sockpp::tcp_socket &sock);
};