#include "WorldGeneratorFlat.h"

#include "BlockList.h"

World *WorldGeneratorFlat::generateWorld(int seed) {
    auto world = new World();
    int Xmin = -10;
    int Xmax = -Xmin;
    int chunks = -(Xmin - Xmax) + 1;
    int i = 0;

    world->_seed = seed;

    while(i < chunks) {
        world->_chunks.push_back(generateChunk((Xmin + i) * 16, seed, i));
        world->_chunksTotal++;
        i++;
        int percent = ((float)i / (float)chunks) * 100;
        std::cout << percent << "\% complete" << std::endl;
    }

    return world;
}
Chunk *WorldGeneratorFlat::generateChunk(int worldXposition, int seed, int chunkIndex) {
    int i = 0;
    auto chunk = new Chunk();

    chunk->_worldXposition = worldXposition;
    chunk->_chunkIndex = chunkIndex;

    while(i < 16) {
        auto block = new Block();

        block->_y = 30;
        block->_x = i;
        block->_id = BLOCK_STONE;
        block->_chunk_index = chunkIndex;

        chunk->_blocks.push_back(block);

        i++;
    }

    i = 0;
    while(i < 16) {
        auto block = new Block();

        block->_y = 28;
        block->_x = i;
        block->_id = BLOCK_STONE;
        block->_chunk_index = chunkIndex;

        chunk->_blocks.push_back(block);

        i++;
    }

    return chunk;
}