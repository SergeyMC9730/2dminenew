#include <iostream>
#include <thread>

#include <sockpp/tcp_acceptor.h>

#include <PacketTools.hpp>
#include <ClientRegisterPacket.pb.h>

#include <lists.h>
#include <bandwidth.h>

#include "ServerConnection.h"

sockpp::tcp_acceptor server;

std::vector<ServerConnection *> sockets;

ServerConnection *getConnection(uint32_t handler) {
    int i = 0;
    while(i < sockets.size()) {
        if (sockets[i] != nullptr) {
            if (sockets[i]->_socketID == handler) return sockets[i];
        }
        i++;
    }
    return nullptr;
}
int getConnectionIndex(uint32_t handler) {
    int i = 0;
    while(i < sockets.size()) {
        if (sockets[i] != nullptr) {
            if (sockets[i]->_socketID == handler) return i;
        }
        i++;
    }
    return -1;
}

int main() {
    sockpp::initialize();

    server = sockpp::tcp_acceptor(1111);

    if(!server) {
        std::cout << "Error creating the acceptor: " << server.last_error_str() << std::endl;
		return 1;
    }

    std::cout << "running on port 1111" << std::endl;

    while (true) {
        sockpp::inet_address peer;
        sockpp::tcp_socket sock = server.accept(&peer);

        std::cout << "Connection from " << peer << std::endl;

        if (!sock) {
            std::cout << "Error while accepting connection: " << server.last_error_str() << std::endl;
        } else {
            std::thread thr([](sockpp::tcp_socket sock) {
                sockets.push_back(new ServerConnection(sock));

                PacketTools::sendHelloPacket(sock);

                char *buffer = (char *)malloc(MAX_PACKET_SIZE);
                ssize_t n = 0;

                while(true) {
                    n = sock.read(buffer, MAX_PACKET_SIZE);

                    if (n <= 0) {
                        std::cout << "connection lost" << std::endl;
                        
                        Ball *linkedBall = nullptr;

                        int i = 0;
                        while(i < Lists::balls.size()) {
                            if (Lists::balls[i] != nullptr) {
                                if (Lists::balls[i]->socketID == sock.handle()) {
                                    std::cout << "found linked player" << std::endl;
                                    linkedBall = Lists::balls[i];
                                    Lists::balls[i] = nullptr;
                                    break;
                                }
                            }
                            i++;
                        }

                        if (linkedBall != nullptr) {
                            auto disconnect_event = linkedBall->generateDisconnect();
                            auto packet2 = PacketTools::constructPacket<net::packet::BallDisconnect>(disconnect_event);
                            packet2.set_type(PT_BALL_DISCONNECT);

                            delete linkedBall;
                            linkedBall = nullptr;
                            
                            int index = getConnectionIndex(sock.handle());
                            if (index != -1) {
                                delete sockets[index];
                                sockets[index] = nullptr;
                            } else {
                                std::cout << "Something bad happened: socket is -1!\n";
                            }

                            i = 0;
                            while(i < sockets.size()) {
                                auto another_sock = sockets[i];
                                if (another_sock != nullptr) {
                                    if (another_sock->_socketID != sock.handle()) {
                                        PacketTools::sendPacket(packet2, another_sock->_socket);
                                        // std::cout << "notified " << another_sock->_socket.handle() << " about player update for " << sock.handle() << std::endl;
                                    }
                                }
                                i++;
                            }
                        }
                        free(buffer);
                        return;
                    }

                    net::packet::MetaPacket packet;
                    packet.ParseFromArray(buffer, 600);

                    // std::cout << "Received packet id " << packet.type() << std::endl;

                    switch(packet.type()) {
                        case PT_CLIENT_REGISTER: {
                            auto cl = PacketTools::receivePacket<net::packet::ClientRegisterPacket>(packet);
                            std::cout << "Registered player " << cl.name() << std::endl;

                            int i = 0;
                            
                            while(i < Lists::balls.size()) {
                                auto ball = Lists::balls[i];
                                if (ball != nullptr) {
                                    auto p = ball->generate();
                                    auto meta = PacketTools::constructPacket<net::packet::BallInfo>(p);
                                    meta.set_type(PT_BALL_INFO);
                                    PacketTools::sendPacket(meta, sock);
                                }
                                i++;
                            }
                            break;
                        }
                        case PT_BALL_INFO: {
                            // std::cout << "Got ball data" << std::endl;

                            bool ball_exists = false;
                            int i = 0;
                            Ball *client_ball = nullptr;
                            
                            auto b = PacketTools::receivePacket<net::packet::BallInfo>(packet);

                            while(i < Lists::balls.size()) {
                                auto ball = Lists::balls[i];
                                if (ball != nullptr) {
                                    if (b.id() == ball->getID()) {
                                        ball_exists = true;
                                        client_ball = ball;
                                        break;
                                    }
                                }
                                i++;
                            }

                            if (ball_exists) {
                                if (client_ball->socketID == sock.handle()) {
                                    client_ball->x = b.x();
                                    client_ball->y = b.y();
                                }
                            } else {
                                client_ball = new Ball(b);
                                client_ball->socketID = sock.handle();
                                Lists::balls.push_back(client_ball);
                            }

                            // notify everyone about player

                            i = 0;
                            while(i < sockets.size()) {
                                auto another_sock = sockets[i];
                                if (another_sock != nullptr) {
                                    if (another_sock->_socketID != sock.handle()) {
                                        auto constructed = PacketTools::constructPacket<net::packet::BallInfo>(b);
                                        constructed.set_type(PT_BALL_INFO);
                                        PacketTools::sendPacket(constructed, another_sock->_socket);
                                        // std::cout << "notified " << another_sock->_socket.handle() << " about player update for " << sock.handle() << std::endl;
                                    }
                                }
                                i++;
                            }
                            break;
                        }
                        default: {
                            break;
                        }
                    }

                    // PacketTools::sendEmptyPacket(PT_OK, sock);

                    bzero(buffer, MAX_PACKET_SIZE);
                }

                // sock.
            }, std::move(sock.clone()));

            thr.detach();
        }        
    }

    return 0;
}