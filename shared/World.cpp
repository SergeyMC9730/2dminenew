#include "World.h"

net::packet::WorldTransmitPacket World::generate() {
    net::packet::WorldTransmitPacket packet;
    packet.set_chunkarraysize(this->_chunks.size());
    return packet;
}