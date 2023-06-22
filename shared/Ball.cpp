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

#include "Ball.h"

#include <cstdlib>

Ball::Ball() {
    unique_id = rand();
    client = true;
}
Ball::Ball(net::packet::BallInfo ball) {
    unique_id = ball.id();
    x = ball.x();
    y = ball.y();
}

net::packet::BallInfo Ball::generate() {
    net::packet::BallInfo ball;

    ball.set_id(unique_id);
    ball.set_x(x);
    ball.set_y(y);

    return ball;
}
net::packet::BallDisconnect Ball::generateDisconnect() {
    net::packet::BallDisconnect ball;

    ball.set_id(unique_id);

    return ball;
}

uint32_t Ball::getID() {
    return unique_id;
}

bool Ball::isClient() {
    return client;
}