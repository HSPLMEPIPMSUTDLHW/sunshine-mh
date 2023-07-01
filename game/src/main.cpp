#include "rlImGui.h"
#include "Physics.h"
#include "Collision.h"
#include "Tilemap.h"
#include "Pathfinder.h"
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "rlImGui.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

Tilemap map;
Pathfinder pathfinder;

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
        height = map.GetTileWidth();

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

        TileCoord mouseTile = map.GetTileAtScreenPos(GetMousePosition());
        if (map.tileInBounds(mouseTile))
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                pathfinder = Pathfinder(&map, player.pos, TileCoord(mouseTile));
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                pathfinder = Pathfinder(&map, player.pos, TileCoord(mouseTile));
                pathfinder.SolvePath();
            }
        }
        if (pathfinder.map != nullptr)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                pathfinder.ProcessNextIterationFunctional();
            }
            //(drawPath)
        }
        /*
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && map.isTileTraversable(tile) )
        {      
            player.pos = tile;
        }
      

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {

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
                        DrawRectangleV(map.GetScreenPosOfTile(t), { map.GetTileWidth(), map.GetTileHeight() }, PINK);
                      //  cout << t.x << " | " << t.y << " [] " << map.GetScreenPosOfTile(tile  ).x << " | " << map.GetScreenPosOfTile(tile  ).y << endl;
                      //  cout << map.GetScreenPosOfTile(tile + NORTH).x << " | " << map.GetScreenPosOfTile(tile + NORTH).y << endl;
                 //       std::cout << t.x << "|" << t.y << " [] ";
                      // cout << map.GetScreenPosOfTile(tile+NORTH).y << " N " << map.GetScreenPosOfTile(tile + SOUTH).y << " S " << map.GetScreenPosOfTile(tile+EAST).x << " E " << map.GetScreenPosOfTile(tile+WEST).x << " W " << endl;
                    }
                    // std::cout << std::endl;
                }
            }
            
        }
        */

        map.Draw();
        map.DrawPaths();
        if (IsKeyDown(KEY_E))
        {
            
            std::cout << "yeah " << endl;
            for (TileCoord pos : map.GetAllTraversableTiles())
            {
                DrawRectangleV(map.GetScreenPosOfTile(pos), { map.GetTileWidth(), map.GetTileHeight() }, PINK);
            }
        }
        player.Draw();

        EndDrawing();
    }
   

    CloseWindow();
    return 0;
}