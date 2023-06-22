#include "WorldRenderer.h"

#include <raylib.h>

#include <BlockList.h>

void WorldRenderer::render(World *world) {
    int i = 0;
    while(i < world->_chunks.size()) {
        int j = 0;
        auto chunk = world->_chunks[i];

        while(j < chunk->_blocks.size()) {
            Vector2 pos;
            auto block = chunk->_blocks[j];

            pos.x = (chunk->_worldXposition + block->_x) * 32;
            pos.y = block->_y * 32;

            switch(block->_id) {
                default: {}
                case BLOCK_STONE: {
                    DrawRectangle(pos.x, pos.y, 32, 32, GRAY);
                    //DrawLine(pos.x, pos.y, pos.x + 32, pos.y - 32, GRAY);
                    break;
                }
                case BLOCK_AIR: {
                    break;
                }
            }

            j++;
        }

        i++;
    }
}