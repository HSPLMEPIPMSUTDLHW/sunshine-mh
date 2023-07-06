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
/*
struct tileAgent
{
    TileCoord pos;
    Vector2 posV;
    Color color;
    float height;
    TileCoord step;

    tileAgent(Vector2 start, Color c)
    {
        pos = start;
        posV = start;
        color = c;
        height = map.GetTileWidth();

    }

    void moveToStep()
    {
        TileCoord s = pos - step;
        Vector2 toMove = { (float)s.x,(float)s.y };
        Vector2 v = { toMove.x / 10, toMove.y / 10 };
        posV = posV - v;
    }
    void SetStep(TileCoord s)
    {
        posV = { (float)pos.x,(float)pos.y };
        step = map.GetScreenPosOfTile(s);
    }
    void setPos(TileCoord p)
    {

        pos  =  p;
        step = p;
        posV = { (float)pos.x,(float)pos.y };
    }
    bool finishedStep()
    {
     //   cout << "POSV: " << posV.x << "|" << posV.y << " STEP: " << (float)step.x<<  "|" << (float)step.y << endl;
        Vector2 length = { posV.x - (float)step.x, posV.y = (float)step.y } ;
        cout << "LENGTH" << Length(length) << endl;
        return ((Length(length) >= 1)|| ((pos.x = posV.x) && (pos.y = posV.y)));
    }
    void Update()
    {
        if (!finishedStep()) moveToStep();
        else
        {
            cout << "TRUE!!" << endl;
            (pos = posV);
        }

    }
    void Draw()
    {
        DrawRectangleV(map.GetScreenPosOfTileV(posV), { height, height }, RED);
    }

};*/

struct tileAgent
{
    TileCoord pos;
    Vector2 posV;
    int increment;
    Color color;
    float height;
    TileCoord step;
    bool stepcomplete = true;

    tileAgent(Vector2 start, Color c)
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

void wallBreaker()
{
    vector<TileCoord> tiles = map.GetAllTraversableTiles();
    bool broken = false;
    TileCoord theTile = tiles[0];
    TileCoord theOtherTile = { 14,9 };
    int floors = 0;
    int pos = 0;
    while(!broken)
    {
        pathfinder = Pathfinder(&map, theTile, theOtherTile);
       // pathfinder.SolvePath();
        while (!pathfinder.IsCompleted())
        {
            pathfinder.ProcessNextIterationFunctional();
            floors++;
        }
        cout << "FLOOR: "<< floors << " | " << tiles.size() << endl;
        if (floors >= tiles.size())
        {
            cout << "ALL FLOORS TOUCHED" << endl;
            broken = true;
        }
        else 
        { 
            cout << "FUck" << endl;
            cout << "SEARCH" << endl;
            broken = true;
            bool search = false;
            vector<TileCoord> adjacenttiles;
          
            cout << "SAERCH" << endl;
            for (TileCoord t : tiles)
            {
                cout << "THE " <<map.GetAdjacentWalls(t).size() << endl;
 
                if (map.GetAdjacentWalls(t).size() > 0)
                    adjacenttiles.push_back(t);
            }
            cout << "TIME " << endl;
            pos = rand() % adjacenttiles.size();
            vector<TileCoord> sex = map.GetAdjacentWalls(adjacenttiles[pos]);
            int d = map.GetAdjacentWalls(adjacenttiles[pos]).size();
            int destroy = rand() % (1+map.GetAdjacentWalls(adjacenttiles[pos]).size());
            map.SetTile(map.GetAdjacentWalls(adjacenttiles[pos])[destroy], Tile::Floor);
        }

        }
     
    
  //  pathfinder = Pathfinder(&map, player.pos, TileCoord(SelectedTile));
 //   pathfinder.SolvePath();
  //  path.clear();
}


int main(void)
{
    srand(time(NULL));
    map.clearTiles(Tile::Floor);
  //  map.RandomizeTiles();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    bool useGUI = false;
    SetTargetFPS(60);
    tileAgent player({ 0,0 },RED);
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
        if (player.stepcomplete && !traveling)
        {


        if (IsKeyPressed(KEY_W) && (map.isTileTraversable({ player.pos.x,player.pos.y - 1 })))
        {
            player.setStep(player.pos + map.NORTH);
      //      player.step.y = player.pos.y -=1;
        }
        else if (IsKeyPressed(KEY_S) && (map.isTileTraversable({ player.pos.x,player.pos.y + 1 })))
        {
            player.setStep(player.pos + map.SOUTH);
        }
        else if (IsKeyPressed(KEY_A) && (map.isTileTraversable({ player.pos.x - 1,player.pos.y })))
        {
            player.setStep(player.pos + map.WEST);
        }
        else if (IsKeyPressed(KEY_D) && (map.isTileTraversable({ player.pos.x + 1,player.pos.y })))
        {
            player.setStep(player.pos + map.EAST);
        }

            if (map.tileInBounds(mouseTile))
            {
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                {

                    pathfinder = Pathfinder(&map, player.pos, TileCoord(SelectedTile));
                    path.clear();
                    steps = 0;
                    haspath = false;
             
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    pathfinder = Pathfinder(&map, player.pos, TileCoord(SelectedTile));
                    pathfinder.SolvePath();
                    path.clear();
                    steps = 0;
                    haspath = false;
                    traveling = true;
                }

            }

        }
        
 
        if (IsKeyDown(KEY_R))
        {
            map.SetTile(mouseTile, Tile::Wall);

        }
        if (IsKeyDown(KEY_ENTER) && traveling == false)
        {
            traveling = true;
            steps = 0;
        }

        if (IsKeyDown(KEY_G) && map.isTileTraversable(mouseTile))
        {
            player.setPos(mouseTile);

        }
        if (IsKeyPressed(KEY_U))
        {
            wallBreaker();

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
                

                if (traveling && player.stepcomplete)
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
                            player.step = path[steps];
                            steps++;
                        }
                        else
                        {
                            pathfinder = Pathfinder(&map, player.pos, TileCoord(SelectedTile));
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
 
               // player.Update();
            }
            else
            {
                pathfinder.drawCurrent();
                pathfinder.drawGoal();
            }
        }

        player.Update();
        map.Draw();
 
 

  
        player.Draw();

        EndDrawing(); 
    }
   

    CloseWindow();
    return 0;
}