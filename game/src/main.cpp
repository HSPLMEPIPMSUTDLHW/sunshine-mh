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
    vector<TileCoord> spaceupdate;
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
        if (IsKeyDown(KEY_R))
        {
            map.SetTile(mouseTile, Tile::Wall);

        }


        if (IsKeyDown(KEY_G) && map.isTileTraversable(mouseTile))
        {
            player.pos = mouseTile;
        }
        if (map.tileInBounds(mouseTile))
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                spaceupdate.clear();
                pathfinder = Pathfinder(&map, player.pos, TileCoord(mouseTile));
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
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
            //    pathfinder.drawCurrent();
                spaceupdate.push_back(pathfinder.getCurrent());
    
            }
            if (IsKeyDown(KEY_SPACE))
            {
               
                pathfinder.drawCurrent();
          
            }
            if (pathfinder.IsCompleted())
            {
                
                pathfinder.drawCosts();
                pathfinder.drawSolution();
                if (IsKeyDown(KEY_Y))
                {
                  //  pathfinder.drawSolution();
                    


                }
                pathfinder.drawGoal();

            }
            else
            {
                pathfinder.drawCurrent();
                pathfinder.drawGoal();
            }
        }

        map.Draw();
 
 

  
        player.Draw();

        EndDrawing(); 
    }
   

    CloseWindow();
    return 0;
}