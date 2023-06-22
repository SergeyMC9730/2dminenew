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

#include "network.h"

std::vector<Vector2> balls = {};

int main() {
    int resolution_x = 640;
    int resolution_y = 480;

    InitWindow(resolution_x, resolution_y, "raylib");
    SetTargetFPS(144);

    Ball *b = new Ball();
    b->x = resolution_x / 2;
    b->y = resolution_y / 2;

    Lists::balls.push_back(b);

    //Vector2 ballPosition = { 640.f, 360.f };

    float speed = 1.2f;

    net::network_thread("localhost", 1111);

    while(!WindowShouldClose()) {
        // if (IsKeyDown(KEY_RIGHT)) {
        //     b->x += speed;
        //     b->modified = true;
        // }
        // if (IsKeyDown(KEY_LEFT)) {
        //     b->x -= speed;
        //     b->modified = true;
        // }
        // if (IsKeyDown(KEY_UP))    {
        //     b->y -= speed;
        //     b->modified = true;
        // }
        // if (IsKeyDown(KEY_DOWN))  {
        //     b->y += speed;
        //     b->modified = true;
        // }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            auto mouse_pos = GetMousePosition();
            b->x = mouse_pos.x;
            b->y = mouse_pos.y;
            b->modified = true;
        }

        int i = 0;

        BeginDrawing();

        ClearBackground(RAYWHITE);

        while(i < Lists::balls.size()) {
            auto ball = Lists::balls[i];
            if (ball != nullptr) {
                Vector2 pos = { (float)ball->x, (float)ball->y };
                DrawCircleV(pos, 50, (ball->isClient()) ? BLUE : MAROON);
            }
            i++;
        }

        DrawFPS(3, 3);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}