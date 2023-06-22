#include "WorldGeneratorGiberrish.h"

#include "BlockList.h"

#include <random>

World *WorldGeneratorGiberrish::generateWorld(int seed) {
    auto world = new World();
    int Xmin = -64;
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
Chunk *WorldGeneratorGiberrish::generateChunk(int worldXposition, int seed, int chunkIndex) {
    int i = 0;
    auto chunk = new Chunk();

    chunk->_worldXposition = worldXposition;
    chunk->_chunkIndex = chunkIndex;

    std::default_random_engine rng;
    rng.seed(seed * chunkIndex * worldXposition);

    std::vector<int> blocks = {
        BLOCK_AIR, BLOCK_STONE
    };

    while(i < 64) {
        int j = 0;
        while(j < 16) {
            bool place = ((rng() % 2) == 0) ? true : false;
            if (place) {
                auto block = new Block();

                block->_x = j;
                block->_y = i;
                block->_id = blocks[rng() % blocks.size()];
                block->_chunk_index = chunkIndex;

                chunk->_blocks.push_back(block);
            }
            j++;
        }
        i++;
        j = 0;
    }

    return chunk;
}