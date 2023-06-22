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

#include <iostream>
#include <thread>

#include <sockpp/tcp_acceptor.h>

#include <PacketTools.hpp>
#include <ClientRegisterPacket.pb.h>

#include <lists.h>
#include <bandwidth.h>

#include "ServerConnection.h"
#include <WorldGeneratorFlat.h>
#include <WorldGeneratorGiberrish.h>

#include <World.h>

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

WorldContainer *wcontainer;

int main() {
    sockpp::initialize();

    wcontainer = new WorldContainer();
    wcontainer->_worldName = "123";
    auto generator = new WorldGeneratorGiberrish();
    wcontainer->_world_overworld = generator->generateWorld(111);

    delete generator;

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

                bool can_continue_world = false;
                bool can_continue_chunk = false;
                bool can_continue_block = false;

                bool registered = false;

                while(true) {
                    n = sock.read(buffer, MAX_PACKET_SIZE);

                    if (n <= 0) {
                        std::cout << "connection lost" << std::endl;
                        
                        Player *linkedPlayer = nullptr;

                        int i = 0;
                        while(i < Lists::players.size()) {
                            if (Lists::players[i] != nullptr) {
                                if (Lists::players[i]->socketID == sock.handle()) {
                                    std::cout << "found linked player" << std::endl;
                                    linkedPlayer = Lists::players[i];
                                    Lists::players[i] = nullptr;
                                    break;
                                }
                            }
                            i++;
                        }

                        if (linkedPlayer != nullptr) {
                            auto disconnect_event = linkedPlayer->generateDisconnect();
                            auto packet2 = PacketTools::constructPacket<net::packet::PlayerDisconnectPacket>(disconnect_event);
                            packet2.set_type(PT_BALL_DISCONNECT);

                            delete linkedPlayer;
                            linkedPlayer = nullptr;
                            
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
                        case PT_WORLD_END: {
                            can_continue_world = true;
                            break;
                        }
                        case PT_CHUNK_END: {
                            can_continue_chunk = true;
                            break;
                        }
                        case PT_BLOCK_END: {
                            can_continue_block = true;
                            break;
                        }
                        case PT_CLIENT_REGISTER: {
                            if (registered) break;

                            registered = true;

                            auto cl = PacketTools::receivePacket<net::packet::ClientRegisterPacket>(packet);
                            std::cout << "Registered player " << cl.name() << std::endl;

                            int i = 0;
                            
                            while(i < Lists::players.size()) {
                                auto player = Lists::players[i];
                                if (player != nullptr) {
                                    auto p = player->generate();
                                    auto meta = PacketTools::constructPacket<net::packet::PlayerInfoPacket>(p);
                                    meta.set_type(PT_BALL_INFO);
                                    PacketTools::sendPacket(meta, sock);
                                }
                                i++;
                            }

                            std::thread ttt([&]() {

                                auto p = wcontainer->_world_overworld->generate();
                                auto meta = PacketTools::constructPacket<net::packet::WorldTransmitPacket>(p);
                                meta.set_type(PT_WORLD_START);

                                // std::cout << "begin world\n";

                                PacketTools::sendPacket(meta, sock);

                                // std::this_thread::sleep_for(std::chrono::milliseconds(100));

                                while(!can_continue_world);
                                can_continue_world = false;

                                i = 0;
                                while(i < wcontainer->_world_overworld->_chunks.size()) {
                                    // std::cout << "begin chunk" << i << "\n";
                                    auto chunk = wcontainer->_world_overworld->_chunks[i];
                                    auto p2 = chunk->generate();
                                    auto meta2 = PacketTools::constructPacket<net::packet::ChunkTransmitPacket>(p2);
                                    meta2.set_type(PT_CHUNK_START);
                                    PacketTools::sendPacket(meta2, sock);

                                    while(!can_continue_chunk);
                                    can_continue_chunk = false;

                                    int j = 0;
                                    while(j < chunk->_blocks.size()) {
                                        // std::cout << "begin block" << j << "\n";
                                        auto block = chunk->_blocks[j];
                                        auto p3 = block->generate();
                                        auto meta3 = PacketTools::constructPacket<net::packet::BlockTransmitPacket>(p3);
                                        meta3.set_type(PT_BLOCK_START);
                                        PacketTools::sendPacket(meta3, sock);
                                        while(!can_continue_block);
                                        can_continue_block = false;
                                        j++;
                                    }

                                    i++;
                                }
                            });
                            ttt.detach();

                            break;
                        }
                        case PT_BALL_INFO: {
                            // std::cout << "Got player data" << std::endl;

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
                                if (client_player->socketID == sock.handle()) {
                                    client_player->x = b.x();
                                    client_player->y = b.y();
                                }
                            } else {
                                client_player = new Player(b);
                                client_player->socketID = sock.handle();
                                Lists::players.push_back(client_player);
                            }

                            // notify everyone about player

                            i = 0;
                            while(i < sockets.size()) {
                                auto another_sock = sockets[i];
                                if (another_sock != nullptr) {
                                    if (another_sock->_socketID != sock.handle()) {
                                        auto constructed = PacketTools::constructPacket<net::packet::PlayerInfoPacket>(b);
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