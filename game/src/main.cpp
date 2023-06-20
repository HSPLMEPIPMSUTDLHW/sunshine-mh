#include "rlImGui.h"
#include <iostream>
#include "Math.h"
 
#include <vector>
#include <string>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = NearestPoint(lineStart, lineEnd, circlePosition);
    return Distance(nearest, circlePosition) <= circleRadius;
}

Vector2 centripetalAccel(Vector2 v, float ac, bool clockwise)
{
    float angle;
    if (clockwise) angle = 90 * DEG2RAD;
    else angle = -90 * DEG2RAD;
    return Rotate(Normalize(v), angle) * ac;

}


//Takes the position of the agent, the target, the current velocity of the agent, the max speed and the max acceleration
// Returns a vector for acceleration that will get the agent to the target that accounts for the current velocity of the agent
Vector2 Seek(const Vector2& agentPos, const Vector2& agentVel, const Vector2& targetPos, const float desiredSpeed, const float accel)
{
    Vector2 targetDistance = { targetPos - agentPos };
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

Vector2 Arrive(const Vector2& agentPos, const Vector2& agentVel, const Vector2& targetPos,  float desiredSpeed, const float accel, float slowingRadius)
{

    Vector2 targetDistance = { targetPos - agentPos };
    Vector2 toTarget = Normalize(targetDistance);

    Vector2 desiredVel = toTarget * desiredSpeed;
    if (sqrt(LengthSqr(targetDistance)) < slowingRadius)
    {
        std::cout << sqrt(LengthSqr(targetDistance)) << std::endl;
        desiredVel = desiredVel * ((LengthSqr(targetDistance)) / slowingRadius);
    }
    Vector2 deltaVel = desiredVel - agentVel;

    Vector2 outputAccel = Normalize(deltaVel) * accel;

    return outputAccel;
}

//Class for the Agent

struct Rigidbody
{
    Vector2 position = { 0 };
    Vector2 velocity = { 0 };
    Vector2 accel = { 0 };
    float width, height, radius;
    float fowardRad = 0;
};

struct Obstacle : public Rigidbody
{
    Color color;
    Obstacle(float x, float y, float w, float h, Color c)
    {
        color = c;
        position = { x,y };
        velocity = { 0,0 };
        accel = { 0,0 };
        height = h;
        width = w;
        radius = h/2;
 
    }
    void Draw()
    {
        DrawCircle(position.x, position.y, radius, color);
    }
};

struct Whisker
{
    Vector2 length = { 0 };
    Vector2 point = { 0 };
    Color whiskerColor = GREEN;
    bool touched;
};

enum Behavior
{
    IDLE = 0,
    SEEK,
    FLEE,
    ARRIVE,
    AVOID,
    Count
};

class Agent : public Rigidbody
{
private:
    Color color;
    float maxSpeed;
    float maxAccel;
    float radar;
    Texture2D sprite;
    Whisker whiskers[4];
    float whiskerLength = 200;
    float ForwardAngle;
    float avoidencePower;
    Behavior behavior;
    Vector2 targetPos;
    float arriveaccelmod;
    std::string name; // Name is like a tag so I can identify objects if they are in a container
public:
    Agent(float x, float y, float w, float h, Color c, std::string n)
    {
        color = c;
        sprite.height = h;
        sprite.width = w;
        height = h;
        width = w;
        radius = h / 2;
        radar = 200;
        position = { x,y };
        velocity = { 0,0 };
        accel = { 0,0 };
        maxSpeed = 100;
        maxAccel = 300;
        name = n;
        
        for (int i = 0; i < 4; i++)
        {
            whiskers[i].whiskerColor = GREEN;
            whiskers[i].touched = false;
        }
       
        
        avoidencePower = 2;
    }

    void Draw()
    {
        DrawCircle(position.x, position.y, radius, color);
        for (int i = 0; i < 4; i++)
        {
            DrawLineV({ position.x , position.y }, (position + getWhiskerPos(i) * whiskerLength), getWhiskerColor(i));
            DrawCircleLines(position.x, position.y, radar, GREEN);
        }
    }
    void Update()
    {
        //When update is called,
       const float dt = GetFrameTime();
       ForwardAngle = atan2f(getVel().y, getVel().x);
       whiskers[0].length = Normalize(Rotate(Vector2{ whiskerLength,0 }, ForwardAngle + 5 * DEG2RAD));
       whiskers[1].length = Normalize(Rotate(Vector2{ whiskerLength,0 }, ForwardAngle - 5 * DEG2RAD));
       whiskers[2].length = Normalize(Rotate(Vector2{ whiskerLength,0 }, ForwardAngle + 15 * DEG2RAD));
       whiskers[3].length = Normalize(Rotate(Vector2{ whiskerLength,0 }, ForwardAngle - 15 * DEG2RAD));
       for (int i = 0; i < 4; i++)
       {
           whiskers[i].touched = false;
       }
       
       float currentspeed = sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
        if (currentspeed > maxSpeed)
        {
            velocity = (Scale(velocity, (maxSpeed / currentspeed)));
        }
        switch (behavior)
        { 
        case IDLE:
        break;
        case SEEK:
            targetPos = GetMousePosition();
            addSeek(targetPos);
        break;
        case FLEE:
            targetPos = GetMousePosition();
            addFlee(targetPos);
        break;
        case ARRIVE:
            targetPos = GetMousePosition();
            addArrive(targetPos);
            
       
            if (sqrt(LengthSqr(position - targetPos)) < radar)
            {
            //    arriveaccelmod = 1 - sqrt(LengthSqr(position - targetPos) / radar);
                //std::cout << sqrt(LengthSqr(position - targetPos)) << std::endl;
            }
            if (sqrt(LengthSqr(position-targetPos)) < 10)
            {
                //   std::cout << sqrt(LengthSqr(targetDistance)) << std::endl;
             //   velocity = velocity * 0;
            }
            break;
        default:
        break;
        }
        position = position + (velocity * dt) + (accel * 0.5f * dt * dt);
        velocity = velocity + accel * dt;
    }

    void addSeek(Vector2 target)
    {
        accel = accel + Seek(position, velocity, target, maxSpeed, maxAccel);
    }
    void addFlee(Vector2 target)
    {
        accel = accel + Flee(position, velocity, target, maxSpeed, maxAccel);
    }
    void addArrive(Vector2 target)
    {
        accel = (accel + Arrive(position, velocity, target, maxSpeed, maxAccel,radar));


    }

    void CollisionAvoidence(Vector2 obs,float w, float ac)
    {
        Vector2 relative = { 1, 0 };
        for (int i = 0; i < 4; i++)
        {
            Vector2 point = NearestPoint(getPos(), { (getPos().x + whiskers[i].length.x * whiskerLength) ,(getPos().y + whiskers[i].length.y * whiskerLength)}, obs);
            Vector2 relativeWhisker = Normalize(Rotate(whiskers[i].length, -ForwardAngle));
            whiskers[i].whiskerColor = GREEN;
            if (!whiskers[i].touched)
            {     
                if (CheckCollisionLineCircle(getPos(), point, obs, 25))
                {
                    whiskers[i].whiskerColor = RED;
                    whiskers[i].touched = true;
                    if (atan2f(relativeWhisker.y, relativeWhisker.x) > 0)
                    {                      
                        setAccel(getAccel() + (centripetalAccel(getVel(), ac, false) * avoidencePower));
                    }
                    else
                    {
                        setAccel(getAccel() + (centripetalAccel(getVel(), ac, true) * avoidencePower));
                    }
                }
            }
            else
            {
                whiskers[i].whiskerColor = RED;
            }    
        }    
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
    void setMaxAccel(float a)
    {
        maxAccel = a;
    }
    float getMaxAccel()
    {
        return maxAccel;
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

    Vector2 getWhiskerPos(int w)
    {
        return whiskers[w].length;
    }
    Color  getWhiskerColor(int w)
    {
        return whiskers[w].whiskerColor;
    }
    void setBehavior(Behavior b)
    {
        behavior = b;
    }
     
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
    static int agentBehavior;
    Vector2 rectpos = { (SCREEN_WIDTH / 2 )- 25,(SCREEN_HEIGHT / 2 )-25};
 
 
    Color circleColor = RED;

    std::vector<Obstacle*> Obstacles;
 

    Agent Player(((SCREEN_WIDTH / 2) - 25), ((SCREEN_HEIGHT / 2) - 25), 50, 50, BLUE, "Blue");
   // Agent Obstacle1(((SCREEN_WIDTH / 8) - 25), ((SCREEN_HEIGHT / 4) - 25), 50, 50, BLACK,"");
   

   // Rectangle playerRec{ Obstacle1.getPos().x, Obstacle1.getPos().y, 50, 50 };
    static Vector2 vel = { 0,0 };
    static Vector2 accel = { 0,0 };
    static float maxSpeed = 20;
    float ac = 0;
    int flee = 1;
    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        Player.setAccel({ 0,0 });


        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello World!", 16, 9, 20, RED);
        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
           
            rlImGuiBegin();

            ImGui::Checkbox("Enable Behavior", &seeking);
            //Other seek function that makes them seek towards or flee from the mouse.
            if (seeking)
            {
                
                ImGui::RadioButton("Seek",&agentBehavior,SEEK);
                ImGui::RadioButton("Flee", &agentBehavior, FLEE);
                ImGui::RadioButton("Arrive", &agentBehavior, ARRIVE);
                Player.setBehavior(static_cast<Behavior>(agentBehavior));
  


            }
            else
            {
                Player.setBehavior(IDLE);
            }
            ImGui::SliderFloat("Seek Accel", &ac, 0, 500); // The max acceleration
            ImGui::SliderFloat("Max Speed", &maxSpeed, 0, 500);
            /*
            for (const auto Agent : Birds)
            {
                Agent->setMaxSpeed(maxSpeed);
            }
            */
            Player.setMaxSpeed(maxSpeed);
            Player.setMaxAccel(ac);

            rlImGuiEnd();
 

        }
        if (IsKeyPressed(KEY_ONE)) agentBehavior = SEEK;
        if (IsKeyPressed(KEY_TWO)) agentBehavior = FLEE;
        if (IsKeyPressed(KEY_THREE)) agentBehavior = ARRIVE;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
          Obstacles.push_back(new Obstacle(GetMousePosition().x, GetMousePosition().y, 50, 50, BLACK));
        }
    

        if (IsKeyDown(KEY_G))
            {
            std::cout << "dt " << dt << std::endl;
        }

        if (CheckCollisionCircleRec(GetMousePosition(), 20, { Player.getPos().x,  Player.getPos().y, Player.getWidth(), Player.getHeight() }))
            circleColor = PINK;
        else
            circleColor = RED;
 
        Vector2 playerPosOffset = { Player.getPos().x + 25,Player.getPos().y + 25 };
        Vector2 defaultAngle = { 150 + Player.getPos().x   ,Player.getPos().y    };
        Vector2 defaultNormal = Vector2{ 175,0 };
 
 

        for (auto obst : Obstacles)
        {
            Player.CollisionAvoidence(obst->position, 25, ac);
        }

        Player.Update();
      
        /*
        if (CheckCollisionPointCircle(point, Obstacle1.getPos(), 25))
        {
            std::cout << "P  " << point.x << "||" << point.y  << std::endl;
        }
        */

        //DrawCircle(Obstacle1.getPos().x, Obstacle1.getPos().y, 25, Obstacle1.getColor());
        for (auto obst : Obstacles)
        {
            obst->Draw();
        }

      //  DrawLineV({ 100,600 }, { 200,600 }, GREEN);
       // DrawLineV({ 100,600 }, { (200 + Forward.x * 100),(600 + Forward.y * 100) }, RED);
    //    DrawLineV({ Player.getPos().x , Player.getPos().y }, defaultAngle, ORANGE);
 

    //    DrawLineV({ Player.getPos().x , Player.getPos().y }, { (Player.getPos().x + Forward.x * 150) ,(Player.getPos().y + Forward.y * 150) }, RED);
        Player.Draw();
     
        DrawLineV({ Player.getPos().x , Player.getPos().y }, GetMousePosition(), BLUE);
        DrawLineV({ Player.getPos().x , Player.getPos().y }, { Player.getVel().x + Player.getPos().x  ,Player.getVel().y + Player.getPos().y }, GREEN);
       
        EndDrawing();
        
    }

    CloseWindow();
    return 0;
}