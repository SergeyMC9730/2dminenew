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