#include "rlImGui.h"
#include <iostream>
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

class Ridgedbody
{
private:
    Ridgedbody();
     
};

Vector2 Normalized(Vector2 vec)
{
    float hyp =  sqrt((vec.x * vec.x )+( vec.y * vec.y)) ;
    Vector2 norm = { vec.x / hyp,vec.y / hyp };
    return norm;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    bool useGUI = false;
    static bool seeking = false;
    static bool fleeing = false;
    Vector2 rectpos = { (SCREEN_WIDTH / 2 )- 25,(SCREEN_HEIGHT / 2 )-25};
    Vector2 targetDistance;
    Vector2 tDNormal;
    Color circleColor = RED;

    Rectangle playerRec{ rectpos.x, rectpos.y, 50, 50 };
    static Vector2 vel = { 0,0 };
    static Vector2 accel = { 0,0 };
    static Vector2 vec1 = { playerRec.x, playerRec.y };
    static float maxSpeed = 20;
    float ac = 0;
    int flee = 1;
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

            ImGui::SliderFloat("Rectangle Vel X", &vel.x, -50, 50);
            ImGui::SliderFloat("Rectangle Vel Y", &vel.y, -50, 50);
            ImGui::Checkbox("Seek", &seeking);
            if (seeking)
            {
                
                ImGui::Checkbox("Fleeing", &fleeing);
                if (fleeing) flee = -1;
                else flee = 1;
                ImGui::SliderFloat("Seek Accel", &ac, 0, 500);
                targetDistance = { GetMousePosition().x - playerRec.x - 25 ,  GetMousePosition().y - playerRec.y - 25 };
                Vector2 toTarget = Normalize(targetDistance);
                Vector2 desiredVel = toTarget * maxSpeed;
                Vector2 deltaVel = desiredVel - vel;
                accel = Normalize(deltaVel) * ac;
              //  float tdhyp = { sqrt(targetDistance.x * targetDistance.x + targetDistance.y * targetDistance.y) };
                /*
                tDNormal = Normalize(targetDistance);

                Vector2 desiredVel = (tDNormal * maxSpeed) +  vel*flee;
                /*                
                accel.x = tDNormal.x * ac;
                accel.y = tDNormal.y * ac;
                */
                //float dvhp = { sqrt(desiredVel.x * desiredVel.x + desiredVel.y * desiredVel.y) };
        //        Vector2 dVNormal = Normalize(desiredVel);// { desiredVel.x / dvhp,desiredVel.y / dvhp };
               /*
                accel.x = dVNormal.x * ac * flee;
                accel.y = dVNormal.y * ac * flee;
                */  /*
               accel.x = desiredVel.x * ac * flee;
               accel.y = desiredVel.y * ac * flee;*/
            }
            else
            { 
            ImGui::SliderFloat("Rectangle Accel X", &accel.x, -50, 50);
            ImGui::SliderFloat("Rectangle Accel Y", &accel.y, -50, 50);
            }
            ImGui::SliderFloat("Max Speed", &maxSpeed, 0, 500);
            if (ImGui::Button("Reset Vel/Accel"))
            {

                vel = { 0,0 }; 
                accel = { 0,0 };
            }
            if (ImGui::Button("Reset Position"))
            {
                playerRec.x = SCREEN_WIDTH / 2 -25;
                playerRec.y = SCREEN_HEIGHT / 2-25;
            }
            rlImGuiEnd();
           // playerRec.x = vec1.x;
          //  playerRec.y = vec1.y;

        }

        playerRec.x += (vel.x * dt) + 0.5f * accel.x*dt*dt;
        playerRec.y += (vel.y * dt) + 0.5f * accel.y * dt * dt;
        vel.x += accel.x*dt;
        vel.y += accel.y*dt;
        float currentspeed = sqrt(vel.x * vel.x + vel.y * vel.y);
        if (currentspeed > maxSpeed)
        {
            vel = Scale (vel ,(maxSpeed / currentspeed));
        }
        if (IsKeyDown(KEY_G))
        {
            std::cout << "dt " << dt << std::endl;
        }
        if (IsKeyPressed(KEY_H))
        {
            std::cout << "targetdistance  " << targetDistance.x <<" | " << targetDistance.y << std::endl;
            std::cout << "target normal  " << tDNormal.x << " | " << tDNormal.y << std::endl;
        }
        if (CheckCollisionCircleRec(GetMousePosition(), 20, playerRec))
            circleColor = PINK;
        else
            circleColor = RED;
        DrawRectangle(playerRec.x, playerRec.y, playerRec.width, playerRec.height, BLUE);
        DrawLineV({ playerRec.x+25, playerRec.y+25 }, { vel.x + playerRec.x +25 ,vel.y + playerRec.y + 25 }, RED);
        DrawLineV({ playerRec.x + 25, playerRec.y + 25 }, { accel.x + playerRec.x + 25 ,accel.y + playerRec.y + 25 }, GREEN);
        DrawCircle(GetMousePosition().x, GetMousePosition().y, 20, circleColor);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}