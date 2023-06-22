#include "World.h"

net::packet::BlockTransmitPacket Block::generate() {
    net::packet::BlockTransmitPacket packet;
    packet.set_x(this->_x);
    packet.set_y(this->_y);
    packet.set_id(this->_id);
    packet.set_chunk_index(this->_chunk_index);
    return packet;
}