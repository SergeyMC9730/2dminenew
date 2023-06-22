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

        int world_chunk_index = 0;

        while (true) {
            n = net::connector.read(buffer, MAX_PACKET_SIZE);

            if (n <= 0) {
                std::cout << "Connection lost" << std::endl;
                return;
            }

            net::packet::MetaPacket packet;
            packet.ParseFromArray(buffer, 1024);

            std::cout << "Received packet id " << packet.type() << std::endl;
        
            switch(packet.type()) {
                case PT_HELLO: {
                    net::packet::ClientRegisterPacket p_authorize;
                    p_authorize.set_name("Player");
                    packet = PacketTools::constructPacket<net::packet::ClientRegisterPacket>(p_authorize);
                    packet.set_type(PT_CLIENT_REGISTER);
                    PacketTools::sendPacket(packet, net::connector);
                    break;
                }
                case PT_WORLD_START: {
                    //auto wdata = PacketTools::receivePacket<net::packet::WorldTransmitPacket>(packet);
                    std::cout << "Creating new world!\n";
                    Lists::world = new World();
                    PacketTools::sendEmptyPacket(PT_WORLD_END, net::connector);
                    break;
                }
                case PT_CHUNK_START: {
                    if (Lists::world != nullptr) {
                        std::cout << "Creating new chunk\n";
                        auto cdata = PacketTools::receivePacket<net::packet::ChunkTransmitPacket>(packet);
                        auto chunk = new Chunk();
                        world_chunk_index = cdata.chunkarrayindex();
                        chunk->_chunkIndex = cdata.chunkarrayindex();
                        chunk->_worldXposition = cdata.worldxposition();
                        Lists::world->_chunks.push_back(chunk);
                    }
                    PacketTools::sendEmptyPacket(PT_CHUNK_END, net::connector);
                    break;
                    // Lists::world
                }
                case PT_BLOCK_START: {
                    if (Lists::world != nullptr) {
                        auto bdata = PacketTools::receivePacket<net::packet::BlockTransmitPacket>(packet);
                    
                        auto block = new Block();
                        block->_chunk_index = bdata.chunk_index();
                        block->_id = bdata.id();
                        block->_x = bdata.x();
                        block->_y = bdata.y();

                        std::cout << "Block " << block->_id << ". Pos: " << block->_x << ":" << block->_y << " in chunk " << block->_chunk_index << "\n";

                        Lists::world->_chunks[world_chunk_index]->_blocks.push_back(block);
                    }
                    PacketTools::sendEmptyPacket(PT_BLOCK_END, net::connector);
                    break;
                }
                case PT_BALL_DISCONNECT: {
                    auto b = PacketTools::receivePacket<net::packet::PlayerDisconnectPacket>(packet);
                    int i = 0;

                    while(i < Lists::players.size()) {
                        auto player = Lists::players[i];
                        if (player != nullptr) {
                            if (b.id() == player->getID()) {
                                delete player;
                                player = nullptr;
                                Lists::players[i] = nullptr;
                                break;
                            }
                        }
                        i++;
                    }

                    break;
                }
                case PT_BALL_INFO: {
                    bool player_exists = false;
                    int i = 0;
                    Player *client_player = nullptr;
                            
                    auto b = PacketTools::receivePacket<net::packet::PlayerInfoPacket>(packet);

                    while(i < Lists::players.size()) {
                        auto player = Lists::players[i];
                        if (player != nullptr) {
                            if (b.id() == player->getID()) {
                                player_exists = true;
                                client_player = player;
                                break;
                            }
                        }
                        i++;
                    }

                    if (player_exists) {
                        if (!client_player->isClient()) {
                            client_player->x = b.x();
                            client_player->y = b.y();
                        }
                    } else {
                        client_player = new Player(b);
                        Lists::players.push_back(client_player);
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
            while(i < Lists::players.size()) {
                if (Lists::players[i] != nullptr) {
                    if (Lists::players[i]->isClient() && Lists::players[i]->modified) {
                        auto playerdata = Lists::players[i]->generate();
                        auto packet = PacketTools::constructPacket<net::packet::PlayerInfoPacket>(playerdata);
                        packet.set_type(PT_BALL_INFO);
                        PacketTools::sendPacket(packet, net::connector);
                        Lists::players[i]->modified = false;
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