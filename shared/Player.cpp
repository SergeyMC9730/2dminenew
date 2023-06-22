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

#include "Player.h"

#include <cstdlib>

Player::Player() {
    unique_id = rand();
    client = true;
}
Player::Player(net::packet::PlayerInfoPacket player) {
    unique_id = player.id();
    x = player.x();
    y = player.y();
}

net::packet::PlayerInfoPacket Player::generate() {
    net::packet::PlayerInfoPacket player;

    player.set_id(unique_id);
    player.set_x(x);
    player.set_y(y);

    return player;
}
net::packet::PlayerDisconnectPacket Player::generateDisconnect() {
    net::packet::PlayerDisconnectPacket player;

    player.set_id(unique_id);

    return player;
}

uint32_t Player::getID() {
    return unique_id;
}

bool Player::isClient() {
    return client;
}