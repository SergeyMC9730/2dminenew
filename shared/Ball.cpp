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