#include "rlImGui.h"
#include <iostream>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    bool useGUI = false;

    Vector2 rectpos = { SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2 };
    Rectangle playerRec{ rectpos.x, rectpos.y, 50, 50 };
    static Vector2 vel = { 0,0 };
    static float accel = 0;
    static Vector2 vec1 = { playerRec.x, playerRec.y };
    
    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
       
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello World!", 16, 9, 20, RED);
        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();
          

        if (ImGui::SliderFloat("Rectangle X", &(playerRec.x), 0, SCREEN_WIDTH))
        {
            //playerRec.x = vec1.x;
        }
        if (ImGui::SliderFloat("Rectangle Y", &(playerRec.y), 0, SCREEN_HEIGHT))
        {
           // playerRec.y = vec1.y;
        }
        if (ImGui::Button("Print"))
        {
            std::cout << "Velocity" << vel.x << "|" << vel.y << std::endl;
        }
            ImGui::SliderFloat("Rectangle Vel X", &vel.x, -20, 20);
            ImGui::SliderFloat("Rectangle Vel Y", &vel.y, -20, 20);
            ImGui::SliderFloat("Rectangle Accel", &accel, -20, 20);
            rlImGuiEnd();
           // playerRec.x = vec1.x;
          //  playerRec.y = vec1.y;

        }
        
    
        playerRec.x += (vel.x * dt) + 0.5f*accel*dt*dt;
        playerRec.y += (vel.y * dt) + 0.5f * accel * dt * dt;
        vel.x += accel*dt;
        vel.y += accel*dt;
        if (IsKeyDown(KEY_G))
        {
            std::cout << "dt " << dt << std::endl;
        }
        DrawRectangle(playerRec.x, playerRec.y, playerRec.width, playerRec.height, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}