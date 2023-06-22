#pragma once

#include <BallInfo.pb.h>
#include <BallDisconnect.pb.h>

class Ball {
protected:
    uint32_t unique_id = 0;
    bool client = false;
public:
    Ball();
    Ball(net::packet::BallInfo ball);

    double x = 0;
    double y = 0;
    uint32_t socketID = 0;

    bool modified = true;

    net::packet::BallInfo generate();
    net::packet::BallDisconnect generateDisconnect();

    uint32_t getID();

    bool isClient();
};