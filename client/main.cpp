/*
    2DMine -- 2D minecraft on raylib
    Copyright (C) 2023  Sergei Baigerov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <thread>
#include <vector>

#include <raylib.h>

#include <lists.h>
#include <WorldGeneratorGiberrish.h>

#include "network.h"
#include "WorldRenderer.h"

std::vector<Vector2> players = {};

int main() {
    int resolution_x = 1920;
    int resolution_y = 1080;

    InitWindow(resolution_x, resolution_y, "raylib");
    SetTargetFPS(144);

    Player *b = new Player();
    b->x = resolution_x / 2;
    b->y = resolution_y / 2;

    Lists::players.push_back(b);

    Vector2 cameraPosition = { 0.f, 0.f };

    float speed = 5.f;

    net::network_thread("localhost", 1111);

    // WorldGeneratorGiberrish *wg = new WorldGeneratorGiberrish();
    // Lists::world = wg->generateWorld(111);
    // delete wg;
    // wg = nullptr;

    Camera2D cam;
    cam.zoom = 0.1f;
    cam.rotation = 0.f;

    while(!WindowShouldClose()) {

        if (IsKeyDown(KEY_RIGHT)) {
            cameraPosition.x += speed;
        }
        if (IsKeyDown(KEY_LEFT)) {
            cameraPosition.x -= speed;
        }
        if (IsKeyDown(KEY_UP)) {
            cameraPosition.y -= speed;
        }
        if (IsKeyDown(KEY_DOWN))  {
            cameraPosition.y += speed;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            auto mouse_pos = GetMousePosition();
            b->x = mouse_pos.x + cameraPosition.x;
            b->x /= cam.zoom;
            b->y = mouse_pos.y + cameraPosition.y;
            b->y /= cam.zoom;
            b->modified = true;
        }

        cam.target = cameraPosition;

        int i = 0;

        BeginDrawing();
        BeginMode2D(cam);

        ClearBackground(RAYWHITE);

        if (Lists::world != nullptr) {
            WorldRenderer::render(Lists::world);
        }

        DrawRectangle(2, 2, 30, 30, BLACK);

        while(i < Lists::players.size()) {
            auto player = Lists::players[i];
            if (player != nullptr) {
                Vector2 pos = { (float)player->x, (float)player->y };
                DrawCircleV(pos, 50, (player->isClient()) ? BLUE : MAROON);
            }
            i++;
        }

        EndMode2D();
        DrawFPS(3, 3);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}