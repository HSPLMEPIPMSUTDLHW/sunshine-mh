#include "rlImGui.h"
#include "Physics.h"
#include "Collision.h"
#include "Tilemap.h"
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "rlImGui.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

Tilemap map;
using namespace std;

struct tileAgent
{
    TileCoord pos;
    Color color;
    float height;
    tileAgent(Vector2 start, Color c)
    {
        pos = start;
        color = c;
        height = map.tileSizeX;

    }
    
    void Draw()
    {
        DrawRectangleV(map.GetScreenPosOfTile(pos), { height, height }, RED);
    }

};

int main(void)
{
    srand(time(NULL));
    map.RandomizeTiles();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    bool useGUI = false;
    SetTargetFPS(60);
    tileAgent player({ 0,0 },RED);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (IsKeyPressed(KEY_W) && (map.isTileTraversable({ player.pos.x,player.pos.y - 1 })))
        {
            player.pos.y -= 1;
        }
        else if (IsKeyPressed(KEY_S) && (map.isTileTraversable({ player.pos.x,player.pos.y + 1 })))
        {
            player.pos.y +=1;
        }
        else if (IsKeyPressed(KEY_A) && (map.isTileTraversable({ player.pos.x-1,player.pos.y })))
        {
            player.pos.x -= 1;
        }
        else if (IsKeyPressed(KEY_D) && (map.isTileTraversable({ player.pos.x + 1,player.pos.y })))
        {
            player.pos.x += 1;
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            TileCoord tile = { GetMousePosition().x / 32,GetMousePosition().y / 32 };
            player.pos = tile;
        }
        map.Draw();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            TileCoord tile = { GetMousePosition().x / 32,GetMousePosition().y / 32 };

            if (tile.x < MAP_WIDTH && tile.y < MAP_HEIGHT)
            {
                // std::cout << tile.x << " | " << tile.y << std::endl;
               //  std::cout << map.isTileTraversable(tile)<<std::endl;
                //std::cout << map.tiles[tile.x][tile.y] << std::endl;
                if (map.isTileTraversable(tile))
                {
                    vector<TileCoord> adjacent = (map.GetAdjacentTiles(tile));
                    for (auto t : adjacent)
                    {
                        DrawRectangleV(map.GetScreenPosOfTile(t), { map.tileSizeX, map.tileSizeX }, PINK);
                      //  cout << t.x << " | " << t.y << " [] " << map.GetScreenPosOfTile(tile  ).x << " | " << map.GetScreenPosOfTile(tile  ).y << endl;
                      //  cout << map.GetScreenPosOfTile(tile + NORTH).x << " | " << map.GetScreenPosOfTile(tile + NORTH).y << endl;
                 //       std::cout << t.x << "|" << t.y << " [] ";
                      // cout << map.GetScreenPosOfTile(tile+NORTH).y << " N " << map.GetScreenPosOfTile(tile + SOUTH).y << " S " << map.GetScreenPosOfTile(tile+EAST).x << " E " << map.GetScreenPosOfTile(tile+WEST).x << " W " << endl;
                    }
                    // std::cout << std::endl;
                }
            }
            
        }
        map.DrawPaths();
        player.Draw();

        EndDrawing();
    }
   

    CloseWindow();
    return 0;
}