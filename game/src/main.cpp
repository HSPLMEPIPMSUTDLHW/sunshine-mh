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
  //  Birds.push_back(new Rigidbody(((SCREEN_WIDTH / 2) - 25), ((SCREEN_HEIGHT / 2) - 25), 50, 50, BLUE,"Blue"));
  //  Birds.push_back(new Rigidbody(((SCREEN_WIDTH / 3) - 25), ((SCREEN_HEIGHT / 3) - 25), 50, 50, ORANGE, "Orange"));

    Rigidbody Player(((SCREEN_WIDTH / 2) - 25), ((SCREEN_HEIGHT / 2) - 25), 50, 50, BLUE, "Blue");
   // Rigidbody Obstacle1(((SCREEN_WIDTH / 8) - 25), ((SCREEN_HEIGHT / 4) - 25), 50, 50, BLACK,"");
  //  Rigidbody Obstacle2(((SCREEN_WIDTH / 4) - 25), ((SCREEN_HEIGHT / 1.3) - 25), 50, 50, BLACK,"");

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
                    /*
                    for (const auto Rigidbody : Birds)
                    {
                        Rigidbody->setAccel(Seek(Rigidbody->getPos(), Rigidbody->getVel(), GetMousePosition() - pOffset, maxSpeed, ac));
                    }
                    */
                    Player.setAccel(Seek(Player.getPos(), Player.getVel(), GetMousePosition() - pOffset, maxSpeed, ac));
                }
                else
                {
                    /*
                    for (const auto Rigidbody : Birds)
                    {
                        Rigidbody->setAccel(Flee(Rigidbody->getPos(), Rigidbody->getVel(), GetMousePosition() - pOffset, maxSpeed, ac));
                    }
                    */
                    Player.setAccel(Flee(Player.getPos(), Player.getVel(), GetMousePosition() - pOffset, maxSpeed, ac));
                }
  


            }
            ImGui::SliderFloat("Seek Accel", &ac, 0, 500); // The max acceleration
            ImGui::SliderFloat("Max Speed", &maxSpeed, 0, 500);
            /*
            for (const auto Rigidbody : Birds)
            {
                Rigidbody->setMaxSpeed(maxSpeed);
            }
            */
            Player.setMaxSpeed(maxSpeed);

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
        /*
        for (const auto Rigidbody : Birds)
        {
            Rigidbody->Update();
        }
        */
        Player.Update();

        if (IsKeyDown(KEY_G))
        {
            std::cout << "dt " << dt << std::endl;
        }

        if (CheckCollisionCircleRec(GetMousePosition(), 20, { Player.getPos().x,  Player.getPos().y, Player.getWidth(), Player.getHeight() }))
            circleColor = PINK;
        else
            circleColor = RED;
        /*
                Vector2 playerPosOffsetNormal = Normalize(Vector2{ Player.getVel().x + 25, Player.getVel().y + 25 });
        Vector2 playerPosOffset = { Player.getVel().x + 25, Player.getVel().y + 25 };
        Vector2 defaultAngle = { 150 + Player.getPos().x + 25 ,Player.getPos().y + 25 };
        float ForwardAngle = atan2f(playerPosOffsetNormal.x, playerPosOffsetNormal.y);
        ForwardAngle = fmodf(ForwardAngle + 2 * PI, 2 * PI);

        Vector2 Forward = Vector2{ cosf(ForwardAngle), sinf(ForwardAngle) };
        DrawRectangle(Player.getPos().x, Player.getPos().y, 50, 50, Player.getColor());
        DrawLineV({ Player.getPos().x + 25, Player.getPos().y + 25 }, playerPosOffset+Forward*100, RED);
        */

        Vector2 playerPosOffset = { Player.getPos().x + 25,Player.getPos().y + 25 };
        Vector2 defaultAngle = { 150 + Player.getPos().x + 25 ,Player.getPos().y + 25 };
        float ForwardAngle = atan2f(Player.getVel().y , Player.getVel().x );
        Vector2 Forward = Normalize(Rotate(Vector2{ 175,0 }, ForwardAngle));
        Vector2 WhiskerLeft = Normalize(Rotate(Vector2{ 175,0 }, ForwardAngle + 45*DEG2RAD));
        Vector2 WhiskerRight = Normalize(Rotate(Vector2{ 175,0 }, ForwardAngle - 45 * DEG2RAD));
        DrawRectangle(Player.getPos().x, Player.getPos().y, 50, 50, Player.getColor());
        DrawLineV({ 100,600 }, { 200,600 }, GREEN);
        DrawLineV({ 100,600 }, { (200 + Forward.x * 100),(600 + Forward.y * 100) }, RED);
        DrawLineV({ Player.getPos().x + 25, Player.getPos().y + 25 }, defaultAngle, ORANGE);
        // DrawLineV({ 0,0 }, GetMousePosition(), BLUE);
        // DrawLineV({ 0,0 }, { (Forward.x * 100),(Forward.y * 100) }, RED);
        DrawLineV({ Player.getPos().x + 25, Player.getPos().y + 25 }, { (Player.getPos().x + Forward.x * 150) + 25,(Player.getPos().y + Forward.y * 150) + 25 }, RED);
        DrawLineV({ Player.getPos().x + 25, Player.getPos().y + 25 }, { (Player.getPos().x + WhiskerLeft.x * 150) + 25,(Player.getPos().y + WhiskerLeft.y * 150) + 25 }, RED);
       // DrawLineV({ Player.getPos().x + 25, Player.getPos().y + 25 }, { (Player.getPos().x + WhiskerLeft.x * 100) + 25,(Player.getPos().y + WhiskerLeft.y * 100) + 25 }, RED);
        DrawLineV({ Player.getPos().x + 25, Player.getPos().y + 25 }, GetMousePosition(), BLUE);
        DrawLineV({ Player.getPos().x + 25, Player.getPos().y + 25 }, { Player.getVel().x + Player.getPos().x + 25 ,Player.getVel().y + Player.getPos().y + 25 }, GREEN);
        if (IsKeyPressed(KEY_H))
        {
            std::cout << "Angle " << ForwardAngle*RAD2DEG << std::endl;
            std::cout << "Angle " << ForwardAngle * DEG2RAD << std::endl;
            std::cout << "Angle " << ForwardAngle << std::endl;
            std::cout << "Forward " << Forward.x << "||" << Forward.y << std::endl;
            std::cout << "Forward " << (Player.getPos().x + 25 + Forward.x) << "||" << (Player.getPos().y + 25 + Forward.y) << std::endl;
        }

       // DrawLineV({ Player.getPos().x + 25, Player.getPos().y + 25 }, { Player.getAccel().x + Player.getPos().x + 25 ,Player.getAccel().y + Player.getPos().y + 25 }, GREEN);
        /*
        for (const auto Rigidbody : Birds)
        {
            if (CheckCollisionCircleRec(GetMousePosition(), 20, { Rigidbody->getPos().x,  Rigidbody->getPos().y, Rigidbody->getWidth(), Rigidbody->getHeight() }))
                circleColor = PINK;
            else
                circleColor = RED;
            DrawRectangle(Rigidbody->getPos().x, Rigidbody->getPos().y, 50, 50, Rigidbody->getColor());
            DrawLineV({ Rigidbody->getPos() .x + 25, Rigidbody->getPos().y + 25 }, { Rigidbody->getVel().x + Rigidbody->getPos().x + 25 ,Rigidbody->getVel().y + Rigidbody->getPos().y + 25 }, RED);
            DrawLineV({ Rigidbody->getPos().x + 25, Rigidbody->getPos().y + 25 }, { Rigidbody->getAccel().x + Rigidbody->getPos().x + 25 ,Rigidbody->getAccel().y + Rigidbody->getPos().y + 25 }, GREEN);
        }
        */
        //DrawRectangle(Obstacle1.getPos().x, Obstacle1.getPos().y, 50, 50, Obstacle1.getColor());
       // DrawRectangle(Obstacle2.getPos().x, Obstacle2.getPos().y, 50, 50, Obstacle2.getColor());

      //  std::cout << "Angle " << ForwardAngle << std::endl;
        EndDrawing();
        
    }

    CloseWindow();
    return 0;
}