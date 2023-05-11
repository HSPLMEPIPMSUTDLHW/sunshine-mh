#include "rlImGui.h"
#include <iostream>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    SetTargetFPS(60);
    int random = 20;
    Color circleColor  = RED;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("RANDOM NUMBER: "+ random, 160, 200, 20, RED);
        DrawText("Hello World!", 16, 9, 20, BLUE);
        DrawCircle(640,360, 40, circleColor);
        DrawCircle(GetMousePosition().x, GetMousePosition().y, 20, circleColor);
        if (CheckCollisionCircles(GetMousePosition(), 20, { 640,360 }, 40))
        {
            std::cout << "TOUCH" << std::endl;
            circleColor = BLUE;
        }
        else
        {
            circleColor = RED;
        }
        // Function 1: IsKeyDown(int)
        // Returns a boolean depending on if a key is being pressed or not (True if pressed, false if not)
        // The input takes an enum that has an int value assigned to it
        // Its use is controlling a program using keyboard inputs, which most games will need in order to play them.
        if (IsKeyPressed(KEY_H))
        {
            
            random = GetRandomValue(0, 2);
            //Function 2:GetRandomValue(int min, int max)
            // Gets a random int from the specified minimum and maximum, inclusive.
            // It can be useful for adding varaity to a game by randomzing elements by changing their values

            // NOTE: maybe remove this one.
            std::cout << "RANDOM NUMBER : " << random << std::endl;
           
        }
        if (IsKeyDown(KEY_SPACE))
        {
            //Function 3: HideCursor and ShowCursor
            //if called, Hide cursor will prevent the mouse curser from showing up while it is hovering over a window
            // it will remain hidden as long as it was called once, unless ShowCurser is called.
            // its use is hiding the curser so it doesn't get in the way of a game window, and showing it when needed, such as for a menu
            HideCursor();
        }
        else
        {
            ShowCursor();
        }
        EndDrawing();
    
    }

    CloseWindow();
    return 0;
} 