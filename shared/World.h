#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "Player.h"

#include <BlockTransmitPacket.pb.h>
#include <ChunkTransmitPacket.pb.h>
#include <WorldTransmitPacket.pb.h>

#pragma pack(push, 1)

class Block {
public:
    uint8_t _x;
    uint8_t _y;
    uint16_t _id;
    uint32_t _chunk_index;

    net::packet::BlockTransmitPacket generate();
};

class Chunk {
public:
    std::vector<Block *> _blocks;
    int _worldXposition;
    int _chunkIndex;

    net::packet::ChunkTransmitPacket generate();
};

class World {
public:
    std::vector<Player *> _players;
    std::vector<Chunk *> _chunks;
    uint32_t _chunksTotal;
    uint32_t _chunksLoaded;
    uint32_t _seed;

    net::packet::WorldTransmitPacket generate();
};

class WorldContainer {
public:
    std::string _worldName;

    World *_world_overworld;
};

#pragma pack(pop)