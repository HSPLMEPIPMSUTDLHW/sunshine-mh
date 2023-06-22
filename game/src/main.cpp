#include "rlImGui.h"
#include <iostream>
#include "Math.h"
 
#include <vector>
#include <string>
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

//Takes the 2 points that represent the start and end of a line, the position of a circle, and its radius. t 
// Returns the point on the line closest to the circle.
bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = NearestPoint(lineStart, lineEnd, circlePosition);
    return Distance(nearest, circlePosition) <= circleRadius;
}

//Returns a normalized vector2 that is 90 degrees left or right of the current velocity and multiplies it by the acceleration specified
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

// Uses the same code for seek, but if the target is within the specified radius of the agent, the agent's desired velocity is multiplied by
// the distance between the agent and target divided by the radius, which causes the desired velocity to shrink as it gets closer.
Vector2 Arrive(const Vector2& agentPos, const Vector2& agentVel, const Vector2& targetPos,  float desiredSpeed, const float accel, float slowingRadius)
{

    Vector2 targetDistance = { targetPos - agentPos };
    Vector2 toTarget = Normalize(targetDistance);

    Vector2 desiredVel = toTarget * desiredSpeed;
    if (sqrt(LengthSqr(targetDistance)) < slowingRadius)
    {
     
        desiredVel = desiredVel * (sqrt(LengthSqr(targetDistance)) / slowingRadius);
    }
    Vector2 deltaVel = desiredVel - agentVel;

    Vector2 outputAccel = Normalize(deltaVel) * accel;

    return outputAccel;
}

//Class for the Rigidbody
// All game objects inherit from this
struct Rigidbody
{
    Vector2 position = { 0 };
    Vector2 velocity = { 0 };
    Vector2 accel = { 0 };
    float width, height, radius;
   // float fowardRad = 0;
};

//Class for Obstacles
// The objects placed with Avoid and the predators placed with Flee use this class
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

//Class for food
// The objects placed with arrive uses this class
// The only difference between Food and Obstacle is food has HP
struct Food : public Rigidbody
{
    int HP = 100;
    Color color;
    Food(float x, float y, float w, float h, Color c)
    {
        color = c;
        position = { x,y };
        velocity = { 0,0 };
        accel = { 0,0 };
        height = h;
        width = w;
        radius = h / 2;

    }
    void Draw()
    {
        DrawCircle(position.x, position.y, radius, color);
    }
};

//Struct for Whiskers
// Because I had many variables that were tied to the whiskers, I found it easier to put them all into one object
struct Whisker
{
    Vector2 length = { 0 };
    Vector2 point = { 0 };
    Color whiskerColor = GREEN;
    bool touched;
};

// enum used to determine the current mouse mode
// Depending on which one the static variable in main is set too, the clicking the mouse will do different things
// The agents also have a behavior variable that when set to SEEK, they will home in on the mouse regardless of range.
enum Behavior
{
    IDLE = 0,
    SEEK,
    FLEE,
    ARRIVE,
    AVOID,
    Count
};

//Class for the agent
class Agent : public Rigidbody
{
private:
    Color color;
    float maxSpeed;
    float maxAccel;
    float radar; //The whisker's length is also determined by the radar radius. The example had them that way, which looked nicer to me.
    float ForwardAngle;
    Texture2D sprite;
    Whisker whiskers[4]; // The 4 whiskers for the agent.
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
        maxSpeed = 400;
        maxAccel = 600;
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
        /* I was unable to get textures working
        Rectangle src = { 0,0,sprite.width, sprite.height };
        Rectangle dst = { position.x, position.y, sprite.width, sprite.height };
        DrawTexturePro(sprite, src, dst, { static_cast<float>(sprite.width)  , static_cast<float>(sprite.height) }, ForwardAngle * RAD2DEG, { 25,25,55 });
        */
        for (int i = 0; i < 4; i++)
        {
           
            DrawLineV({ position.x , position.y }, (position + getWhiskerPos(i) * radar), getWhiskerColor(i));
          

        }
        DrawCircleLines(position.x, position.y, radar, GREEN);
    }
    void Update()
    {
        //When update is called,
       const float dt = GetFrameTime();
       ForwardAngle = atan2f(getVel().y, getVel().x);
       whiskers[0].length = Normalize(Rotate(Vector2{ radar,0 }, ForwardAngle + 5 * DEG2RAD));
       whiskers[1].length = Normalize(Rotate(Vector2{ radar,0 }, ForwardAngle - 5 * DEG2RAD));
       whiskers[2].length = Normalize(Rotate(Vector2{ radar,0 }, ForwardAngle + 15 * DEG2RAD));
       whiskers[3].length = Normalize(Rotate(Vector2{ radar,0 }, ForwardAngle - 15 * DEG2RAD));
       //Sets the vector of the whiskers
       for (int i = 0; i < 4; i++)
       {
           //sets that they have not touched an obstacle
           whiskers[i].touched = false;
       }
       
        // Seek towards the mouse if the behavoir is set to SEEK
       if (behavior == SEEK)
       {
           targetPos = GetMousePosition();
           addSeek(targetPos);
       }
       // Update kinematics
        position = position + (velocity * dt) + (accel * 0.5f * dt * dt);
        velocity = velocity + accel * dt;
        //Gets the current speed to see if it is larger then the maximun speed. If it is, multiplies the velocity by the max speed divided by the current speed
        //
        float currentspeed = sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
        if (currentspeed > maxSpeed)
        {
            velocity = (Scale(velocity, (maxSpeed / currentspeed)));
        }
        // If the position of the agent is out of the screen bounds (So less then 0 or greater then the set width and height)
        // Places the agent on the opposite end of the screen while keeping the velocity and acceleration the same
        if (position.x > SCREEN_WIDTH+25)
        {
            position.x = 0;
        }
        if (position.x < -25)
        {
            position.x = SCREEN_WIDTH;
        }
        if (position.y > SCREEN_HEIGHT + 25)
        {
            position.y = 0;
        }
        if (position.y < -25)
        {
            position.y = SCREEN_HEIGHT;
        }
        accel = Vector2Zero();
    }
    //Calls the seek, flee or arrive functions and adds them to the acceleration
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
    // Function for the collision of the whiskers and obstacles.
    //For each whisker, finds the point on the line closest to the obstacle
    // Checks if that whisker is already touching an obstacle. If it is, skip it. If its not, check if it is touching an obstacle.
    // If it is touching an obstacle, find if its a left or right whisker relative to the forward angle of the agent, and rotate it left or right
    // depending on if the right or left whisker was touched.
    // Note: because it checks it for all whiskers, if all of them are being touched, the agent will just go forward.
    // Ideally I could make it so it checks if two whiskers on each side are being touched, and if they are, rotate it in one direction.
    void CollisionAvoidence(Vector2 obs,float w, float ac)
    {
        for (int i = 0; i < 4; i++)
        {
            Vector2 point = NearestPoint(getPos(), { (getPos().x + whiskers[i].length.x * radar) ,(getPos().y + whiskers[i].length.y * radar)}, obs);
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
    //Get and Set functions
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
    float getRadius()
    {
        return radius;
    }
    float getRadar()
    {
        return radar;
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
    void setTexture(const char* t)
    {
        sprite = LoadTexture(t);
    }
    //Checks if the distance between a vector and the agent's position is less then the length of the radar radius
    bool radarDetection(Vector2 ob, float rad)
    {
        if (sqrt(LengthSqr(position - ob)) < rad)
        {
            return true;
        }
        else return false;
    }
     
};

// Main function
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    bool useGUI = false;
    static int agentBehavior;
    Vector2 rectpos = { (SCREEN_WIDTH / 2 )- 25,(SCREEN_HEIGHT / 2 )-25};
 
 
    Color circleColor = RED;

    std::vector<Agent*> Fish;
    std::vector<Obstacle*> Obstacles;
    std::vector<Food*> Foods;
    std::vector<Obstacle*> Predator;

    
 
    Fish.push_back(new Agent(((SCREEN_WIDTH / 2) - 25), ((SCREEN_HEIGHT / 2) - 25), 50, 50, BLUE, ""));
    Fish.push_back(new Agent(((SCREEN_WIDTH / 3) - 25), ((SCREEN_HEIGHT / 3) - 25), 50, 50, BLUE, ""));
    Fish.push_back(new Agent(((SCREEN_WIDTH / 6) - 25), ((SCREEN_HEIGHT / 2) - 25), 50, 50, BLUE, ""));
  
    // The acceleration starts at 0, so the agents will not move until the acceleration is changed with the Imgui menu
    static float maxSpeed = 20;
    float ac = 0;
   
    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(RAYWHITE);
 
        // The buttons in the imgui menu can change the behavior. Number keys 1 through 4 can also change it
        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {      
            rlImGuiBegin();
             
            ImGui::RadioButton("Seek",&agentBehavior,SEEK);
            ImGui::RadioButton("Flee", &agentBehavior, FLEE);
            ImGui::RadioButton("Arrive", &agentBehavior, ARRIVE);
            ImGui::RadioButton("Avoid", &agentBehavior, AVOID);
            
            ImGui::SliderFloat("Seek Accel", &ac, 0, 500); // The max acceleration
            ImGui::SliderFloat("Max Speed", &maxSpeed, 0, 500);
            rlImGuiEnd();
        }
        if (IsKeyPressed(KEY_ONE)) agentBehavior = SEEK;
        if (IsKeyPressed(KEY_TWO)) agentBehavior = FLEE;
        if (IsKeyPressed(KEY_THREE)) agentBehavior = ARRIVE;
        if (IsKeyPressed(KEY_FOUR)) agentBehavior = AVOID;
        // Creates an object at the position of the mouse. The object created depends on the current behavior.
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && agentBehavior == 2)
        {
            Predator.push_back(new Obstacle(GetMousePosition().x, GetMousePosition().y, 50, 50, GRAY));
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&&agentBehavior == 3)
        {
          Foods.push_back(new Food(GetMousePosition().x, GetMousePosition().y, 50, 50, BROWN));
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && agentBehavior == 4)
        {
            Obstacles.push_back(new Obstacle(GetMousePosition().x, GetMousePosition().y, 50, 50, BLACK));
        }

        //Updating all the fish
        for (const auto Agent : Fish)
        {
            //Sets the agent's max speed and max acceleration
            Agent->setMaxSpeed(maxSpeed);
            Agent->setMaxAccel(ac);
            //Sets the agent's behavior to SEEK if the mouse button is clicked. The agent then seeks towards the mouse through the addseek function
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && agentBehavior == SEEK)
            {
                Agent->setBehavior(SEEK);
            }
            else Agent->setBehavior(IDLE);//Idle is just used like a boolean for the agents to tell them to not seek the mouse
            //Goes through the obstacle vector and calls CollisionAvoidence to check if any obstacles are touching the whiskers
            for (auto obst : Obstacles)
            {
                Agent->CollisionAvoidence(obst->position, 25, ac);
            }
            //Goes through the food vector to check if any are within the range of the radar.
            // If there is, call addArrive to arrive to the food. If the food is within the agent's radius (The blue circle, not the radar), the HP value of the
            // food decreases. When it gets to zero, it is deleted.
            for (auto f : Foods)
            {
                if (Agent->radarDetection(f->position, Agent->getRadar()))
                {
                    Agent->addArrive(f->position);
                    if (Agent->radarDetection(f->position, Agent->getRadius()))
                    {
                        std::cout << "FOOD AT " << f->position.x << "||" << f->position.y << std::endl;
                        f->HP -= 1;
                    }
                }

            }
            //Goes through the Predator vector to check if any are within the range of the radar.
          // If there is, call addFlee with the predator's position to move the agent away from it.
            for (auto P : Predator)
            {
                if (Agent->radarDetection(P->position, Agent->getRadar()))
                {

                    Agent->addFlee(P->position);
                }

            }
            // Call update to update the kinematics
            Agent->Update();
        }
    

    
      
        // For loop for deleting food
        // I did not want to put this in the for loop for the agents, because in my experience with game fundementals,
        //Deleting objects from a vector in a loop that uses the object can cause some errors.
        for(int e = 0; e < Foods.size(); e++)
        {
            if (Foods[e]->HP <= 0)
            {
                delete Foods[e];
                Foods[e] = nullptr;
                Foods.erase(Foods.begin() + e);
                Foods.shrink_to_fit();
            }
        }
 
        //Draws all the objects.
        for (auto obst : Obstacles)
        {
            obst->Draw();
        }
        for (auto f : Foods)
        {
            f->Draw();
        }
        for (auto P : Predator)
        {
            P->Draw();
        }
 
        for (const auto Agent : Fish)
        {
            Agent->Draw();
            DrawLineV({ Agent->getPos().x , Agent->getPos().y }, { Agent->getVel().x + Agent->getPos().x  ,Agent->getVel().y + Agent->getPos().y }, RED);
        }
      
       
        EndDrawing();
        
    }

    CloseWindow();
    return 0;
}