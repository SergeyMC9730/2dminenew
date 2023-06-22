#pragma once

#include "World.h"

class WorldGenerator {
public:
    virtual World *generateWorld(int seed);
    virtual Chunk *generateChunk(int worldXposition, int seed, int chunkIndex);
};