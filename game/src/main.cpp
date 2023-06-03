#include "rlImGui.h"
#include <iostream>
#include "Math.h"
#include <vector>
#include <string>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

//Class for the Rigidbody

class Rigidbody
{
private:
    Vector2 position;
    Vector2 velocity;
    Vector2 accel; 
    Color color;
    float width, height;
    float maxSpeed;
    Rectangle dst;
    std::string name; // Name is like a tag so I can identify objects if they are in a container
public:
    Rigidbody(float x, float y, float w, float h, Color c, std::string n)
    {
        color = c;
        dst = { x,y,w,h};
        position = { x,y };
        velocity = { 0,0 };
        accel = { 0,0 };
        maxSpeed = 0;
        name = n;
    }

    void Update()
    {
        //When update is called,
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

    Color getColor()
    {
        return color;
    }
    std::string getName()
    {
        return name;
    }


     
};

//Takes the position of the agent, the target, the current velocity of the agent, the max speed and the max acceleration
// Returns a vector for acceleration that will get the agent to the target that accounts for the current velocity of the agent
Vector2 Seek(const Vector2& agentPos, const Vector2& agentVel, const Vector2& targetPos, const float desiredSpeed, const float accel)
{
    Vector2 targetDistance = { targetPos-agentPos };
    Vector2 toTarget = Normalize(targetDistance);
    Vector2 desiredVel = toTarget * desiredSpeed;
    Vector2 deltaVel = desiredVel - agentVel;
    Vector2 outputAccel = Normalize(deltaVel) * accel;
  
    return outputAccel;
}
//Takes everything for seek, but makes the max acceleration negative
//This causes the agent to accelerate away from the target.
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
    Vector2 pOffset = { 25,25 };

    
    std::vector<Rigidbody*> Birds;
    Birds.push_back(new Rigidbody(((SCREEN_WIDTH / 2) - 25), ((SCREEN_HEIGHT / 2) - 25), 50, 50, BLUE,"Blue"));
    Birds.push_back(new Rigidbody(((SCREEN_WIDTH / 3) - 25), ((SCREEN_HEIGHT / 3) - 25), 50, 50, ORANGE, "Orange"));

    Rigidbody Obstacle1(((SCREEN_WIDTH / 8) - 25), ((SCREEN_HEIGHT / 4) - 25), 50, 50, BLACK,"");
    Rigidbody Obstacle2(((SCREEN_WIDTH / 4) - 25), ((SCREEN_HEIGHT / 1.3) - 25), 50, 50, BLACK,"");

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
            //Unused Function
        }
        if (ImGui::SliderFloat("Rectangle Y", &(playerRec.y), 0, SCREEN_HEIGHT))
        {
            //Unused Function
        }


            ImGui::SliderFloat("Rectangle Vel X", &vel.x, -50, 50);
            ImGui::SliderFloat("Rectangle Vel Y", &vel.y, -50, 50);
            ImGui::Checkbox("Seek", &seeking);
            //Other seek function that makes them seek towards or flee from the mouse.
            if (seeking)
            {
            
                ImGui::Checkbox("Fleeing", &fleeing);
                if (!fleeing)
                { 
                    for (const auto Rigidbody : Birds)
                    {
                        Rigidbody->setAccel(Seek(Rigidbody->getPos(), Rigidbody->getVel(), GetMousePosition() - pOffset, maxSpeed, ac));
                    }
                    
                }
                else
                {
                    for (const auto Rigidbody : Birds)
                    {
                        Rigidbody->setAccel(Flee(Rigidbody->getPos(), Rigidbody->getVel(), GetMousePosition() - pOffset, maxSpeed, ac));
                    }
                }
  


            }
            else
            { 
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    for (const auto Rigidbody : Birds)
                    {
                        // If the left mouse button is being held down, The agents seek towards the mouse 
                        Rigidbody->setAccel(Seek(Rigidbody->getPos(), Rigidbody->getVel(), GetMousePosition() - pOffset, maxSpeed, ac));
                    }
                }
                else
                {                
                    for (const auto Rigidbody : Birds)
                    {
                   
                        //Checks which obstacle is closer, then sets it to the target
                        // The target's position is used in the Flee function and the agent flees from it
                        Vector2 targetObs;
                        if (sqrtf((Obstacle1.getPos().x - Rigidbody->getPos().x) * (Obstacle1.getPos().x - Rigidbody->getPos().x)
                            + (Obstacle1.getPos().y - Rigidbody->getPos().y) * (Obstacle1.getPos().y - Rigidbody->getPos().y)) <
                            sqrtf((Obstacle2.getPos().x - Rigidbody->getPos().x) * (Obstacle2.getPos().x - Rigidbody->getPos().x)
                                + (Obstacle2.getPos().y - Rigidbody->getPos().y) * (Obstacle2.getPos().y - Rigidbody->getPos().y)))
                        {
                            targetObs = Obstacle1.getPos();
                            std::cout << Rigidbody->getName() << ": OBS1: " << targetObs.x << "|" << targetObs.y << "|  " << sqrtf((Obstacle1.getPos().x - GetMousePosition().x) * (Obstacle1.getPos().x - GetMousePosition().x)
                                + (Obstacle1.getPos().y - GetMousePosition().y) * (Obstacle1.getPos().y - GetMousePosition().y)) << std::endl;
                        }
                        else
                        {
                            targetObs = Obstacle2.getPos();
                            std::cout << Rigidbody->getName() << ": OBS2: " << targetObs.x << "|" << targetObs.y << "|  " << sqrtf((Obstacle1.getPos().x - GetMousePosition().x) * (Obstacle1.getPos().x - GetMousePosition().x)
                                + (Obstacle1.getPos().y - GetMousePosition().y) * (Obstacle1.getPos().y - GetMousePosition().y)) << std::endl;
                        }
                        Rigidbody->setAccel(Flee(Rigidbody->getPos(), Rigidbody->getVel(), { targetObs.x-pOffset.x, targetObs.y - pOffset.y }, maxSpeed, ac));
                        
                    }
                }
            }
            ImGui::SliderFloat("Seek Accel", &ac, 0, 500); // The max acceleration
            ImGui::SliderFloat("Max Speed", &maxSpeed, 0, 500);
            for (const auto Rigidbody : Birds)
            {
                Rigidbody->setMaxSpeed(maxSpeed);
            }
            if (ImGui::Button("Reset Vel/Accel"))
            {

                vel = { 0,0 }; 
                accel = { 0,0 };
            }
            if (ImGui::Button("Reset Position"))
            {
        
            }
            rlImGuiEnd();
 

        }

        for (const auto Rigidbody : Birds)
        {
            Rigidbody->Update();
        }
;
        if (IsKeyDown(KEY_G))
        {
            std::cout << "dt " << dt << std::endl;
        }
        if (IsKeyPressed(KEY_H))
        {
            std::cout << "targetdistance  " << targetDistance.x <<" | " << targetDistance.y << std::endl;
            std::cout << "target normal  " << tDNormal.x << " | " << tDNormal.y << std::endl;
        }
     
        for (const auto Rigidbody : Birds)
        {
            if (CheckCollisionCircleRec(GetMousePosition(), 20, { Rigidbody->getPos().x,  Rigidbody->getPos().y, Rigidbody->getWidth(), Rigidbody->getHeight() }))
                circleColor = PINK;
            else
                circleColor = RED;
            DrawRectangle(Rigidbody->getPos().x, Rigidbody->getPos().y, 50, 50, Rigidbody->getColor());
            DrawLineV({ Rigidbody->getPos().x + 25, Rigidbody->getPos().y + 25 }, { Rigidbody->getVel().x + Rigidbody->getPos().x + 25 ,Rigidbody->getVel().y + Rigidbody->getPos().y + 25 }, RED);
            DrawLineV({ Rigidbody->getPos().x + 25, Rigidbody->getPos().y + 25 }, { Rigidbody->getAccel().x + Rigidbody->getPos().x + 25 ,Rigidbody->getAccel().y + Rigidbody->getPos().y + 25 }, GREEN);
        }
        DrawRectangle(Obstacle1.getPos().x, Obstacle1.getPos().y, 50, 50, Obstacle1.getColor());
        DrawRectangle(Obstacle2.getPos().x, Obstacle2.getPos().y, 50, 50, Obstacle2.getColor());
        EndDrawing();
        
    }

    CloseWindow();
    return 0;
}