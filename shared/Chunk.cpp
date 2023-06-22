#include "World.h"

net::packet::ChunkTransmitPacket Chunk::generate() {
    net::packet::ChunkTransmitPacket packet;
    packet.set_blockarraysize(this->_blocks.size());
    packet.set_chunkarrayindex(this->_chunkIndex);
    packet.set_worldxposition(this->_worldXposition);
    return packet;
}