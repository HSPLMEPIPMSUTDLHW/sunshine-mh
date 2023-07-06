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

struct tileCharacter
{
    TileCoord pos;
    Vector2 posV;
    int increment;
    Color color;
    float height;
    TileCoord step;
    bool stepcomplete = true;

    tileCharacter(Vector2 start, Color c)
    {
        pos = start;
        posV = map.GetScreenPosOfTile(start);
        color = c;
        height = map.GetTileWidth();
        increment = 16;

    }
    bool finishedStep()
    {
         
        return (posV == map.GetScreenPosOfTile(step));
    }
    void Update()
    {
        if (!finishedStep())
        {
           
           
            movingtoStep();
            stepcomplete = false;
            
        }
        else
        {
            pos = map.GetTileAtScreenPos(posV);
            stepcomplete = true;
 
        }

    }
    void setStep(TileCoord s)
    {
        step = s;
        posV = map.GetScreenPosOfTile(pos);
    }
    void moveToStep()
    {
        TileCoord s = pos - step;
 
        pos = pos- s;
    }
    void movingtoStep()
    {
        TileCoord s = pos - step;
        s.x = s.x * (map.GetTileWidth()/ increment);
        s.y = s.y * (map.GetTileHeight()/ increment);
        posV = posV - s.toVec2();
       // cout << "POSV: " << posV.x << "|" << posV.y << " STEP: " << s.x << "|" << s.y << endl;

    }
    void setPos(TileCoord p)
    {
        pos = p;
        posV = map.GetScreenPosOfTile(p);
        step = p;
    }
    void Draw()
    {
        DrawRectangleV(posV, { height, height }, RED);
    }

};

 


int main(void)
{
    srand(time(NULL));
    map.clearTiles(Tile::Wall);
    map.loadTiles();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    bool useGUI = false;
    SetTargetFPS(60);
    tileCharacter monster({ 0,0 },RED);
    vector<TileCoord> spaceupdate;
    std::vector<TileCoord> path;
    int steps = 0;
    bool haspath = false;
    bool traveling = false;
    //TileCoord mouseTile;
    TileCoord SelectedTile;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        TileCoord mouseTile = map.GetTileAtScreenPos(GetMousePosition());
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            SelectedTile = mouseTile;
        }
        if (monster.stepcomplete && !traveling)
        {


        if (IsKeyPressed(KEY_W) && (map.isTileTraversable({ monster.pos.x,monster.pos.y - 1 })))
        {
            monster.setStep(monster.pos + map.NORTH);
      //      monster.step.y = monster.pos.y -=1;
        }
        else if (IsKeyPressed(KEY_S) && (map.isTileTraversable({ monster.pos.x,monster.pos.y + 1 })))
        {
            monster.setStep(monster.pos + map.SOUTH);
        }
        else if (IsKeyPressed(KEY_A) && (map.isTileTraversable({ monster.pos.x - 1,monster.pos.y })))
        {
            monster.setStep(monster.pos + map.WEST);
        }
        else if (IsKeyPressed(KEY_D) && (map.isTileTraversable({ monster.pos.x + 1,monster.pos.y })))
        {
            monster.setStep(monster.pos + map.EAST);
        }

            if (map.tileInBounds(mouseTile))
            {
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                {

                    pathfinder = Pathfinder(&map, monster.pos, TileCoord(SelectedTile));
                    path.clear();
                    steps = 0;
                    haspath = false;
             
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    pathfinder = Pathfinder(&map, monster.pos, TileCoord(SelectedTile));
                    pathfinder.SolvePath();
                    path.clear();
                    steps = 0;
                    haspath = false;
                    traveling = true;
                }

            }

        }
        
        if (IsKeyPressed(KEY_O))
        {
            map.loadTiles();
        }
        if (IsKeyDown(KEY_R))
        {
            map.SetTile(mouseTile, Tile::Wall);

        }
        if (IsKeyDown(KEY_T))
        {
            map.SetTile(mouseTile, Tile::Floor);

        }
        if (IsKeyDown(KEY_ENTER) && traveling == false)
        {
            traveling = true;
            steps = 0;
        }

        if (IsKeyDown(KEY_G) && map.isTileTraversable(mouseTile))
        {
            monster.setPos(mouseTile);

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
                

                if (traveling && monster.stepcomplete)
                {
                  
                    if (!haspath && (steps < pathfinder.GetSolution().size()))
                    {
                       // std::list<TileCoord> sol = pathfinder.GetSolution();
                        for (auto p : pathfinder.GetSolution())
                        {
                            path.push_back(p);
                        }
                        steps = 0;
                        haspath = true;
                    }
                    if (steps < pathfinder.GetSolution().size())
                    {
                      //  cout << "STEPS " << steps << endl;
                        if (map.isTileTraversable(path[steps]))
                        {
                            monster.step = path[steps];
                            steps++;
                        }
                        else
                        {
                            pathfinder = Pathfinder(&map, monster.pos, TileCoord(SelectedTile));
                            pathfinder.SolvePath();
                            path.clear();
                            steps = 0;
                            haspath = false;
                        }
                    }
                    else
                    {
                        traveling = false;
                    }
                    
                }
                pathfinder.drawCosts();
                pathfinder.drawGoal();
                pathfinder.drawSolution();
 
               // monster.Update();
            }
            else
            {
                pathfinder.drawCurrent();
                pathfinder.drawGoal();
            }
        }

        monster.Update();
        map.Draw();
 
 

  
        monster.Draw();

        EndDrawing(); 
    }
   

    CloseWindow();
    return 0;
}