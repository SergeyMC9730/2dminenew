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

#include <sockpp/tcp_connector.h>

#include <cstdint>
#include <string>

namespace net {
    extern sockpp::tcp_connector connector;
    extern bool connected;

    bool connect(std::string host, uint16_t port);
    void network_thread(std::string host, uint16_t port);
}