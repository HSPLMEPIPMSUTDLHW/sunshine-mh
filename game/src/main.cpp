#include "rlImGui.h"
#include "raymath.h"
#include <iostream>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    SetTargetFPS(60);
    int random;
    float circleX = GetScreenWidth() / 2;
    float circleY = GetScreenHeight() / 3;
    //Function 1: GetScreenWidth() and GetScreenHeight()
    // Returns the width and the height of the screen in pixels as an int
    // Useful if you need to get the value of the screen size if you didn't have variables defining them

    Color circleColor  = RED;
    //Function 2: Font LoadFont(const char *fileName);
    //Loads a font from a file
    // Useful if you want to use a font besides the default font and use multiple fonts
    // LoadFontEx also exists, which takes in other parameters to specify the base font size and the amount of characters the font has
    Font ArielFont = LoadFont("../game/assets/textures/arial.ttf");

    random = GetRandomValue(0, 2);
    std::cout << "RANDOM NUMBER : " << random << std::endl;
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndMode3D();
        DrawText("Hello World!", 16, 9, 20, BLUE);
        DrawTextEx(ArielFont, "Hello World!", { 16, 90 }, 32,1, BLUE);

        DrawCircle(circleX,circleY, 40, circleColor);
        DrawCircle(GetMousePosition().x, GetMousePosition().y, 20, circleColor);
        // Function 3: DrawCircle(int centerX, int centerY, float radius, Color color)
        // Draws a Circle at the specified X and Y position with the specified radius and color
        // Can be useful for testing to show an objects hit box or a place holder, since most completed games
        // will likely use their own textures.
        DrawFPS( 1200, 20);
        //Function 4: DrawFFPS ( int xpos, int ypos)
        // Draws text stating the Frames per second
        // Useful for checking if the window's frame rate drops and by how much

        if (CheckCollisionCircles(GetMousePosition(), 20, { circleX,circleY }, 40))
        {
            std::cout << "TOUCH" << std::endl;
            circleColor = BLUE;
        }
        else
        {
            circleColor = RED;
        }
        //Function 5: CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
        // Returns a boolean and takes 2 sets of 3 ints, two for a circle's x and y position, and one for its radius
        // Returns true if the circles radius overlap, returns false otherwise
        // Useful for checking the collision of two circles, which can be helpful if hit boxes of some game objects are circular
        // and because writing a collision function for circles from scratch can be tricky

        // Function 6: IsKeyDown(int)
        // Returns a boolean depending on if a key is being pressed or not (True if pressed, false if not)
        // The input takes an enum that has an int value assigned to it
        // Its use is controlling a program using keyboard inputs, which most games will need in order to play them.

        if (IsKeyPressed(KEY_R))
        {
           // Function 7: Vector2Distance(Vector2 v1, Vector2 v2);   
        // Returns a float based on the distance between two specified vectors
        // Useful when you need to know the distance between two elements in a game
            std::cout << "CIRCLE DISTANCE : " << Vector2Distance(GetMousePosition(), { 640,360 }) << std::endl;

        }
        if (IsKeyDown(KEY_SPACE))
        {
            //Function 8: HideCursor and ShowCursor
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