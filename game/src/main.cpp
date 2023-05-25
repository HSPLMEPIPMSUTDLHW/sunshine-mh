#include "rlImGui.h"
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
    while (!WindowShouldClose())
    {
        
       
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello World!", 16, 9, 20, RED);
        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();
            static Vector2 vec1 = {playerRec.x, playerRec.y};

        if (ImGui::SliderFloat("Rectangle X", &vec1.x, 0, SCREEN_WIDTH))
        {
            playerRec.x = vec1.x;
        }
        if (ImGui::SliderFloat("Rectangle Y", &vec1.y, 0, SCREEN_HEIGHT))
        {
            playerRec.y = vec1.y;
        }
            ImGui::SliderFloat("Rectangle Vel X", &vel.x, -20, 20);
            ImGui::SliderFloat("Rectangle Vel Y", &vel.y, -20, 20);
            ImGui::SliderFloat("Rectangle Accel", &accel, -20, 20);
            rlImGuiEnd();
           // playerRec.x = vec1.x;
          //  playerRec.y = vec1.y;

        }
        vel.x += accel;
        vel.y += accel;
        playerRec.x += vel.x;
        playerRec.y += vel.y;
        DrawRectangle(playerRec.x, playerRec.y, playerRec.width, playerRec.height, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}