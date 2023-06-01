#include "rlImGui.h"
#include <iostream>
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

class Ridgedbody
{
private:
    Vector2 position;
    Vector2 velocity;
    Vector2 accel; 
    Color color;
    float width, height;
    float maxSpeed;
    Rectangle dst;
public:
    Ridgedbody(float x, float y, float w, float h, Color c)
    {
        color = c;
        dst = { x,y,w,h};
        position = { x,y };
        velocity = { 0,0 };
        accel = { 0,0 };
        maxSpeed = 0;
    }

    void Update()
    {
       const float dt = GetFrameTime();
       float currentspeed = sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
        if (currentspeed > maxSpeed)
        {
            velocity = (Scale(velocity, (maxSpeed / currentspeed)));
        }
        position = position + (velocity * dt) + (accel * 0.5f * dt * dt);
        velocity = velocity + accel * dt;
    }

    void setAccel(Vector2 a)
    {
        accel = a;
    }
    Vector2 getAccel()
    {
        return accel;
    }
    void setPos(Vector2 v)
    {
        position = v;
    }
    Vector2 getPos()
    {
        return position;
    }
    Vector2 getVel()
    {
        return velocity;
    }
    void setVel(Vector2 v)
    {
        velocity = v;
    }
    void setMaxSpeed(float m)
    {
        maxSpeed = m;
    }
    float getHeight()
    {
        return height;
    }
    float getWidth()
    {
        return width;
    }


     
};

Vector2 Seek(const Vector2& agentPos, const Vector2& agentVel, const Vector2& targetPos, const float desiredSpeed, const float accel)
{
    Vector2 targetDistance = { targetPos-agentPos };
    Vector2 toTarget = Normalize(targetDistance);
    Vector2 desiredVel = toTarget * desiredSpeed;
    Vector2 deltaVel = desiredVel - agentVel;
    Vector2 outputAccel = Normalize(deltaVel) * accel;
   std::cout << "target " << targetPos.x << "|" << targetPos.y << std::endl;
    return outputAccel;
}
Vector2 Flee(const Vector2& agentPos, const Vector2& agentVel, const Vector2& targetPos, const float desiredSpeed, const float accel)
{   
    return Seek(agentPos, agentVel, targetPos, desiredSpeed, -accel);
}


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


    Ridgedbody player(((SCREEN_WIDTH / 2) - 25), ((SCREEN_HEIGHT / 2) - 25), 50, 50, RED);


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
            std::cout << "Velocity" << player.getVel().x << "|" << player.getVel().y << std::endl;
        }

            ImGui::SliderFloat("Rectangle Vel X", &vel.x, -50, 50);
            ImGui::SliderFloat("Rectangle Vel Y", &vel.y, -50, 50);
            ImGui::Checkbox("Seek", &seeking);
            if (seeking)
            {
                Vector2 pOffset = { 25,25 };
                ImGui::Checkbox("Fleeing", &fleeing);
                if (!fleeing)
                { 
                    player.setAccel(Seek(player.getPos(), player.getVel(), GetMousePosition() - pOffset, maxSpeed, ac));
                }
                else
                {
                    player.setAccel(Flee(player.getPos(), player.getVel(), GetMousePosition() - pOffset, maxSpeed, ac));
                }
  
                ImGui::SliderFloat("Seek Accel", &ac, 0, 500);

                /* 
                targetDistance = { GetMousePosition().x - player.getPos().x - 25 ,  GetMousePosition().y - player.getPos().y - 25 };
                Vector2 toTarget = Normalize(targetDistance);
                Vector2 desiredVel = toTarget * maxSpeed;
                Vector2 deltaVel = desiredVel - player.getVel();
                player.setAccel(Normalize(deltaVel) * ac)  ;
                */
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
            player.setMaxSpeed(maxSpeed);
            if (ImGui::Button("Reset Vel/Accel"))
            {

                vel = { 0,0 }; 
                accel = { 0,0 };
            }
            if (ImGui::Button("Reset Position"))
            {
                player.setPos({ SCREEN_WIDTH / 2 - 25,SCREEN_HEIGHT / 2 - 25 });
            }
            rlImGuiEnd();
           // playerRec.x = vec1.x;
          //  playerRec.y = vec1.y;

        }
        /*
        player.setPos(player.getPos() + ((player.getVel() * dt) + (player.getAccel() * 0.5f * dt * dt)));
       // playerRec.x += (vel.x * dt) + 0.5f * accel.x*dt*dt;
      //  playerRec.y += (vel.y * dt) + 0.5f * accel.y * dt * dt;
        player.setVel(player.getVel() + player.getAccel() * dt);
    //    vel.x += accel.x*dt;
       // vel.y += accel.y*dt;
        float currentspeed = sqrt((player.getVel().x * (player.getVel().x)) + (player.getVel().y * (player.getVel().y)));
        if (currentspeed > maxSpeed)
        {
            player.setVel(Scale(player.getVel(), (maxSpeed / currentspeed)));
        }
        */
        player.Update();
        if (IsKeyDown(KEY_G))
        {
            std::cout << "dt " << dt << std::endl;
        }
        if (IsKeyPressed(KEY_H))
        {
            std::cout << "targetdistance  " << targetDistance.x <<" | " << targetDistance.y << std::endl;
            std::cout << "target normal  " << tDNormal.x << " | " << tDNormal.y << std::endl;
        }
        if (CheckCollisionCircleRec(GetMousePosition(), 20, { player.getPos().x, player.getPos().y,player.getWidth(),player.getHeight() }))
            circleColor = PINK;
        else
            circleColor = RED;
        DrawRectangle(player.getPos().x, player.getPos().y, 50, 50, BLUE);
        DrawLineV({ player.getPos().x+25, player.getPos().y+25 }, { player.getVel().x + player.getPos().x +25 ,player.getVel().y + player.getPos().y + 25 }, RED);
        DrawLineV({ player.getPos().x + 25, player.getPos().y + 25 }, { player.getAccel().x + player.getPos().x + 25 ,player.getAccel().y + player.getPos().y + 25 }, GREEN);
        DrawCircle(GetMousePosition().x, GetMousePosition().y, 20, circleColor);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}