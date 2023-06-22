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

#include "network.h"

#include <iostream>
#include <thread>
#include <cstdlib>
#include <chrono>

#include <MetaPacket.pb.h>
#include <ClientRegisterPacket.pb.h>
#include <PacketTools.hpp>
#include <bandwidth.h>

#include <lists.h>

sockpp::tcp_connector net::connector;
bool net::connected = false;

void net::network_thread(std::string host, uint16_t port) {
    std::thread thr([&](std::string h, uint16_t p) {
        if (!net::connect(h, p)) return;

        net::connected = true;

        void *buffer = malloc(MAX_PACKET_SIZE);
        ssize_t n = 0;

        while (true) {
            n = net::connector.read(buffer, MAX_PACKET_SIZE);

            if (n <= 0) {
                std::cout << "Connection lost" << std::endl;
                return;
            }

            net::packet::MetaPacket packet;
            packet.ParseFromArray(buffer, 1024);

            // std::cout << "Received packet id " << packet.type() << std::endl;
        
            switch(packet.type()) {
                case PT_HELLO: {
                    net::packet::ClientRegisterPacket p_authorize;
                    p_authorize.set_name("Player");
                    packet = PacketTools::constructPacket<net::packet::ClientRegisterPacket>(p_authorize);
                    packet.set_type(PT_CLIENT_REGISTER);
                    PacketTools::sendPacket(packet, net::connector);
                    break;
                }
                case PT_BALL_DISCONNECT: {
                    auto b = PacketTools::receivePacket<net::packet::BallDisconnect>(packet);
                    int i = 0;

                    while(i < Lists::balls.size()) {
                        auto ball = Lists::balls[i];
                        if (ball != nullptr) {
                            if (b.id() == ball->getID()) {
                                delete ball;
                                ball = nullptr;
                                Lists::balls[i] = nullptr;
                                break;
                            }
                        }
                        i++;
                    }

                    break;
                }
                case PT_BALL_INFO: {
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
                        if (!client_ball->isClient()) {
                            client_ball->x = b.x();
                            client_ball->y = b.y();
                        }
                    } else {
                        client_ball = new Ball(b);
                        Lists::balls.push_back(client_ball);
                    }

                    break;
                }
                default: {
                    break;
                }
            }

            bzero(buffer, 1024);
        }
    }, host, port);

    std::thread thr2([&]() {
        while(!net::connected);

        while(true) {
            int i = 0;
            while(i < Lists::balls.size()) {
                if (Lists::balls[i] != nullptr) {
                    if (Lists::balls[i]->isClient() && Lists::balls[i]->modified) {
                        auto balldata = Lists::balls[i]->generate();
                        auto packet = PacketTools::constructPacket<net::packet::BallInfo>(balldata);
                        packet.set_type(PT_BALL_INFO);
                        PacketTools::sendPacket(packet, net::connector);
                        Lists::balls[i]->modified = false;
                    }
                }
                i++;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    });

    thr.detach();
    thr2.detach();
}

bool net::connect(std::string host, uint16_t port) {
    sockpp::initialize();

    net::connector = sockpp::tcp_connector({host, port});

    if(!net::connector) {
        std::cerr << "Error connecting to server at "
			<< sockpp::inet_address("localhost", 1111)
			<< "\n\t" << net::connector.last_error_str() << std::endl;
		return false;
    }

    std::cout << "Connected to the server" << std::endl;

    return true;
}