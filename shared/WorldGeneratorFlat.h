#pragma once

#include "WorldGenerator.h"

class WorldGeneratorFlat : public WorldGenerator {
public:
    World *generateWorld(int seed) override;
    Chunk *generateChunk(int worldXposition, int seed, int chunkIndex) override;
};