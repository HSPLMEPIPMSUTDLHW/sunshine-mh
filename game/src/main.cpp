#include "rlImGui.h"
#include "Physics.h"
#include "Collision.h"

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

constexpr int GRID_LENGTH = 80;
constexpr int GRID_LENGTH_SQR = GRID_LENGTH * GRID_LENGTH;
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int TILE_WIDTH = SCREEN_WIDTH / GRID_LENGTH;
constexpr int TILE_HEIGHT = SCREEN_HEIGHT / GRID_LENGTH;

// Logging decisions is overkill cause multiple logs will be made each frame
#define LOG_DECISIONS false
#define LOG_ACTIONS true

size_t ScreenToGrid(Vector2 point);
Vector2 GridToScreen(size_t index);

vector<size_t> OverlapTiles(Rectangle rectangle);
vector<size_t> VisibleTiles(Circle target, float detectionRadius,
    const Obstacles& obstacles, const vector<size_t>& tiles);

void SaveObstacles(const Obstacles& obstacles, const char* path = "../game/assets/data/obstacles.txt");
Obstacles LoadObstacles(const char* path = "../game/assets/data/obstacles.txt");

void SavePoints(const Points& points, const char* path = "../game/assets/data/points.txt");
Points LoadPoints(const char* path = "../game/assets/data/points.txt");

bool IsCollision(Vector2 lineStart, Vector2 lineEnd, const Obstacles& obstacles);
bool ResolveCollisions(Vector2& point, float radius, const Obstacles& obstacles);

Vector2 Avoid(const Rigidbody& rb, float probeLength, float dt, const Obstacles& obstacles);
Vector2 Patrol(const Points& points, const Rigidbody& rb, size_t& index, float maxSpeed, float slowRadius, float pointRadius);

struct EnemyData
{
    float speed;
    float radius;
    size_t point;

    float detectionRadius;
    float combatRadius;
    float probeLength;

    string name;
};

struct PlayerData
{
    float radius;
};

class Node
{
public:
    Node(Rigidbody& self, EnemyData& selfData) : mSelf(self), mSelfData(selfData) {}
    virtual Node* Evaluate(const Rigidbody& enemy, const PlayerData& enemyData,
        const Points& points, const Obstacles& obstacles) = 0;

protected:
    Rigidbody& mSelf;
    EnemyData& mSelfData;
};

class Condition : public Node
{
public:
    Condition(Rigidbody& self, EnemyData& selfData) : Node(self, selfData) {}
    Node* yes = nullptr;
    Node* no = nullptr;
};

class Action : public Node
{
public:
    Action(Rigidbody& self, EnemyData& selfData) : Node(self, selfData) {}
    virtual Node* Evaluate(const Rigidbody& enemy, const PlayerData& enemyData,
        const Points& points, const Obstacles& obstacles) override
    {
        // nullptr because an action node is a leaf!
        return nullptr;
    }
};

class DetectedCondition : public Condition
{
public:
    DetectedCondition(Rigidbody& self, EnemyData& selfData) : Condition(self, selfData) {}
    virtual Node* Evaluate(const Rigidbody& enemy, const PlayerData& enemyData,
        const Points& points, const Obstacles& obstacles) override
    {
        const Circle detectionCircle{ mSelf.pos, mSelfData.detectionRadius };
        const Circle enemyCircle{ enemy.pos, enemyData.radius };
        return CheckCollisionCircles(detectionCircle, enemyCircle) ? yes : no;
    }
};

// The tile search should be discarded and an FoV calculation should be done instead
class VisibleCondition : public Condition
{
public:
    VisibleCondition(Rigidbody& self, EnemyData& selfData) : Condition(self, selfData) {}
    virtual Node* Evaluate(const Rigidbody& enemy, const PlayerData& enemyData,
        const Points& points, const Obstacles& obstacles) override
    {
        // Doesn't take direction/FoV into account. An omniscient AI leads to better gameplay in this case!
        Circle enemyCircle{ enemy.pos, enemyData.radius };
        Vector2 detectionEnd = mSelf.pos + Normalize(enemy.pos - mSelf.pos) * mSelfData.detectionRadius;
        return IsCircleVisible(mSelf.pos, detectionEnd, enemyCircle, obstacles) ? yes : no;
    }
};

class CombatCondition : public Condition
{
public:
    CombatCondition(Rigidbody& self, EnemyData& selfData) : Condition(self, selfData) {}
    virtual Node* Evaluate(const Rigidbody& enemy, const PlayerData& enemyData,
        const Points& points, const Obstacles& obstacles) override
    {
        Circle selfCircle{ mSelf.pos, mSelfData.combatRadius };
        Circle enemyCircle{ enemy.pos, enemyData.radius };
        return CheckCollisionCircles(selfCircle, enemyCircle) ? yes : no;
    }
};

class PatrolAction : public Action
{
public:
    PatrolAction(Rigidbody& self, EnemyData& selfData) : Action(self, selfData) {}
    virtual Node* Evaluate(const Rigidbody& enemy, const PlayerData& enemyData,
        const Points& points, const Obstacles& obstacles) override
    {
        mSelf.acc = Patrol(points, mSelf, mSelfData.point, mSelfData.speed, 200.0f, 100.0f);
#if LOG_ACTIONS
        cout << mSelfData.name + " patrolling" << endl;
#endif
        return nullptr;
    }
};

class VisibilityAction : public Action
{
public:
    VisibilityAction(Rigidbody& self, EnemyData& selfData, Action& fallback) :
        Action(self, selfData), mFallback(fallback) {}
    virtual Node* Evaluate(const Rigidbody& enemy, const PlayerData& enemyData,
        const Points& points, const Obstacles& obstacles) override
    {
        // Seek nearest enemy-visible tile
        Rectangle area = From({ mSelf.pos, mSelfData.detectionRadius });
        vector<size_t> overlap = OverlapTiles(area);
        vector<size_t> visible = VisibleTiles({enemy.pos, enemyData.radius}, mSelfData.detectionRadius, obstacles, overlap);
        
        // Ensure points aren't too close to obstacles
        Points visiblePoints(visible.size());
        for (size_t i = 0; i < visiblePoints.size(); i++)
            visiblePoints[i] = GridToScreen(visible[i]) + Vector2{TILE_WIDTH * 0.5f, TILE_HEIGHT * 0.5f};

#if LOG_ACTIONS
        if (!visiblePoints.empty())
            cout << mSelfData.name + " finding visibility" << endl;
        else
            cout << mSelfData.name + " falling back" << endl;
#endif

        if (!visiblePoints.empty())
        {
            mSelf.acc = Seek(NearestPoint(mSelf.pos, visiblePoints), mSelf, mSelfData.speed);
            return nullptr;
        }
        return mFallback.Evaluate(enemy, enemyData, points, obstacles);
    }

private:
    Action& mFallback;
};

class TargetAction : public Action
{
public:
    TargetAction(Rigidbody& self, EnemyData& selfData) : Action(self, selfData) {}
    virtual Node* Evaluate(const Rigidbody& enemy, const PlayerData& enemyData,
        const Points& points, const Obstacles& obstacles) override
    {
        mSelf.acc = Arrive(enemy.pos, mSelf, mSelfData.speed, 100.0f, 5.0f);
#if LOG_ACTIONS
        cout << mSelfData.name + " moving" << endl;
#endif
        return nullptr;
    }
};

class CloseAttackAction : public Action
{
public:
    CloseAttackAction(Rigidbody& self, EnemyData& selfData) : Action(self, selfData) {}
    virtual Node* Evaluate(const Rigidbody& enemy, const PlayerData& enemyData,
        const Points& points, const Obstacles& obstacles) override
    {
        mSelf.acc = Arrive(enemy.pos, mSelf, mSelfData.speed, 100.0f, 5.0f);
#if LOG_ACTIONS
        cout << mSelfData.name + " attacking" << endl;
#endif
        return nullptr;
    }
};

void Traverse(Node* node,
    const Rigidbody& enemy, const PlayerData& enemyData, const Points& points, const Obstacles& obstacles)
{
    while (node != nullptr)
    {
        node = node->Evaluate(enemy, enemyData, points, obstacles);
    }
}

int main(void)
{
    Obstacles obstacles = LoadObstacles();
    Points points = LoadPoints();
    size_t point = 0;

    Circle player{ {}, 60.0f };
    Vector2 playerDirection{ 1.0f, 0.0f };
    const float playerRotationSpeed = 100.0f;

    Rigidbody cce;
    cce.pos = { 1000.0f, 250.0f };
    cce.dir = { -1.0f, 0.0f };
    cce.angularSpeed = DEG2RAD * 100.0f;

    Rigidbody rce;
    rce.pos = { 10.0f, 10.0f };
    rce.dir = { 1.0f, 0.0f };
    rce.angularSpeed = DEG2RAD * 100.0f;

    EnemyData ccd;
    ccd.point = 0;
    ccd.speed = 300.0f;
    ccd.radius = 50.0f;
    ccd.detectionRadius = 300.0f;
    ccd.probeLength = 100.0f;
    ccd.combatRadius = 100.0f;
    ccd.name = "Close-combat enemy";

    EnemyData rcd;
    rcd.point = 0;
    rcd.speed = 300.0f;
    rcd.radius = 50.0f;
    rcd.detectionRadius = 300.0f;
    rcd.probeLength = 100.0f;
    rcd.combatRadius = 400.0f;
    rcd.name = "Ranged-combat enemy";

    DetectedCondition cceIsPlayerDetected(cce, ccd);
    VisibleCondition cceIsPlayerVisible(cce, ccd);
    CombatCondition cceIsPlayerCombat(cce, ccd);
    PatrolAction ccePatrol(cce, ccd);
    VisibilityAction cceFindVisibility(cce, ccd, ccePatrol);
    TargetAction cceArrive(cce, ccd);
    CloseAttackAction cceAttack(cce, ccd);

    Node* cceRoot = &cceIsPlayerDetected;
    cceIsPlayerDetected.no = &ccePatrol;
    cceIsPlayerDetected.yes = &cceIsPlayerVisible;
    cceIsPlayerVisible.no = &cceFindVisibility;
    cceIsPlayerVisible.yes = &cceIsPlayerCombat;
    cceIsPlayerCombat.no = &cceArrive;
    cceIsPlayerCombat.yes = &cceAttack;

    const Color background = RAYWHITE;
    const Color playerColor = { 0, 228, 48, 128 };          // GREEN

    const Color cceColor = { 0, 121, 241, 128 };            // BLUE
    const Color cceOverlapColor = { 0, 82, 172, 128 };      // DARKBLUE
    const Color cceVisibleColor = { 102, 191, 255, 128 };   // SKYBLUE

    const Color rceColor = { 135, 60, 190, 128 };           // VIOLET
    const Color rceOverlapColor = { 112, 31, 126, 128 };    // DARKPURPLE
    const Color rceVisibleColor = { 200, 122, 255, 128 };   // PURPLE

    bool useDebug = true;
    bool useGUI = false;
    bool showPoints = false;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    rlImGuiSetup(true);
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        const float playerRotationDelta = playerRotationSpeed * dt * DEG2RAD;

        // Update player information
        if (IsKeyDown(KEY_E))
            playerDirection = Rotate(playerDirection, playerRotationDelta);
        if (IsKeyDown(KEY_Q))
            playerDirection = Rotate(playerDirection, -playerRotationDelta);

        player.position = GetMousePosition();
        const Vector2 playerEnd = player.position + playerDirection * 500.0f;

        //Melee(cce, ccd, { player.position }, { player.radius }, points, obstacles);
        Traverse(cceRoot, { player.position }, { player.radius }, points, obstacles);
        cce.acc = cce.acc + Avoid(cce, ccd.probeLength, dt, obstacles);
        Integrate(cce, dt);

        rce.acc = Patrol(points, rce, point, rcd.speed, 200.0f, 100.0f);
        rce.acc = rce.acc + Avoid(rce, rcd.probeLength, dt, obstacles);
        Integrate(rce, dt);

        bool playerCollision = ResolveCollisions(player.position, player.radius, obstacles);
        bool cceCollision = ResolveCollisions(cce.pos, ccd.radius, obstacles);
        bool rceCollision = ResolveCollisions(rce.pos, ccd.radius, obstacles);

        vector<size_t> cceOverlapTiles = OverlapTiles(From({ cce.pos, ccd.detectionRadius }));
        vector<size_t> rceOverlapTiles = OverlapTiles(From({ rce.pos, rcd.detectionRadius }));
        vector<size_t> cceVisibleTiles = VisibleTiles(player, ccd.detectionRadius, obstacles, cceOverlapTiles);
        vector<size_t> rceVisibleTiles = VisibleTiles(player, rcd.detectionRadius, obstacles, rceOverlapTiles);

        vector<Vector2> intersections;
        for (const Circle& obstacle : obstacles)
        {
            Vector2 poi;
            if (CheckCollisionLineCircle(player.position, playerEnd, obstacle, poi))
                intersections.push_back(poi);
        }
        bool playerIntersection = !intersections.empty();

        BeginDrawing();
        ClearBackground(background);

        // Render debug
        if (useDebug)
        {
            for (size_t i : cceOverlapTiles)
                DrawRectangleV(GridToScreen(i), { TILE_WIDTH, TILE_HEIGHT }, cceOverlapColor);

            for (size_t i : cceVisibleTiles)
                DrawRectangleV(GridToScreen(i), { TILE_WIDTH, TILE_HEIGHT }, cceVisibleColor);

            for (size_t i : rceOverlapTiles)
                DrawRectangleV(GridToScreen(i), { TILE_WIDTH, TILE_HEIGHT }, rceOverlapColor);

            for (size_t i : rceVisibleTiles)
                DrawRectangleV(GridToScreen(i), { TILE_WIDTH, TILE_HEIGHT }, rceVisibleColor);
        }

        // Render entities
        DrawCircleV(cce.pos, ccd.radius, cceCollision ? RED : cceColor);
        DrawCircleV(rce.pos, rcd.radius, rceCollision ? RED : rceColor);
        DrawCircleV(player.position, player.radius, playerCollision ? RED : playerColor);
        DrawLineEx(player.position, playerEnd, 10.0f, playerIntersection ? RED : playerColor);
        DrawLineEx(cce.pos, cce.pos + cce.dir * ccd.detectionRadius, 10.0f, cceColor);
        DrawLineEx(rce.pos, rce.pos + rce.dir * rcd.detectionRadius, 10.0f, rceColor);

        // Avoidance lines
        DrawLineEx(cce.pos, cce.pos + Rotate(Normalize(cce.vel), -30.0f * DEG2RAD) * ccd.probeLength, 5.0f, cceColor);
        DrawLineEx(cce.pos, cce.pos + Rotate(Normalize(cce.vel), -15.0f * DEG2RAD) * ccd.probeLength, 5.0f, cceColor);
        DrawLineEx(cce.pos, cce.pos + Rotate(Normalize(cce.vel),  15.0f * DEG2RAD) * ccd.probeLength, 5.0f, cceColor);
        DrawLineEx(cce.pos, cce.pos + Rotate(Normalize(cce.vel),  30.0f * DEG2RAD) * ccd.probeLength, 5.0f, cceColor);
        DrawLineEx(rce.pos, rce.pos + Rotate(Normalize(rce.vel), -30.0f * DEG2RAD) * rcd.probeLength, 5.0f, rceColor);
        DrawLineEx(rce.pos, rce.pos + Rotate(Normalize(rce.vel), -15.0f * DEG2RAD) * rcd.probeLength, 5.0f, rceColor);
        DrawLineEx(rce.pos, rce.pos + Rotate(Normalize(rce.vel),  15.0f * DEG2RAD) * rcd.probeLength, 5.0f, rceColor);
        DrawLineEx(rce.pos, rce.pos + Rotate(Normalize(rce.vel),  30.0f * DEG2RAD) * rcd.probeLength, 5.0f, rceColor);

        // Render obstacle intersections
        Vector2 obstaclesPoi;
        if (NearestIntersection(player.position, playerEnd, obstacles, obstaclesPoi))
            DrawCircleV(obstaclesPoi, 10.0f, playerIntersection ? RED : playerColor);

        // Render obstacles
        for (const Circle& obstacle : obstacles)
            DrawCircle(obstacle, GRAY);

        // Render points
        for (size_t i = 0; i < points.size(); i++)
        {
            const Vector2& p0 = points[i];
            const Vector2& p1 = points[(i + 1) % points.size()];
            DrawLineV(p0, p1, GRAY);
            DrawCircle(p0.x, p0.y, 5.0f, LIGHTGRAY);
        }
        
        // Render GUI
        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();
            ImGui::Checkbox("Use heatmap", &useDebug);
            ImGui::SliderFloat2("CCE Position", (float*)&cce.pos, 0.0f, 1200.0f);
            ImGui::SliderFloat2("RCE Position", (float*)&rce.pos, 0.0f, 1200.0f);
            ImGui::SliderFloat("CCE Detection Radius", &ccd.detectionRadius, 0.0f, 1500.0f);
            ImGui::SliderFloat("RCE Detection Radius", &rcd.detectionRadius, 0.0f, 1500.0f);
            ImGui::SliderFloat("CCE Probe Length", &ccd.probeLength, 0.0f, 250.0f);
            ImGui::SliderFloat("RCE Probe Length", &rcd.probeLength, 0.0f, 250.0f);
            
            ImGui::Separator();
            if (ImGui::Button("Save Obstacles"))
                SaveObstacles(obstacles);
            if (ImGui::Button("Add Obstacle"))
                obstacles.push_back({ {}, 10.0f });
            if (ImGui::Button("Remove Obstacle"))
                obstacles.pop_back();
            for (size_t i = 0; i < obstacles.size(); i++)
                ImGui::SliderFloat3(string("Obstacle " + to_string(i + 1)).c_str(), (float*)&obstacles[i], 0.0f, 1200.0f);

            ImGui::Separator();
            if (ImGui::Button("Save Points"))
                SavePoints(points);
            if (ImGui::Button("Add Point"))
                points.push_back({ {}, 10.0f });
            if (ImGui::Button("Remove Point"))
                points.pop_back();
            for (size_t i = 0; i < points.size(); i++)
                ImGui::SliderFloat2(string("Point " + to_string(i + 1)).c_str(), (float*)&points[i], 0.0f, 1200.0f);

            rlImGuiEnd();
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}

size_t ScreenToGrid(Vector2 point)
{
    size_t col = point.x / TILE_WIDTH;
    size_t row = point.y / TILE_HEIGHT;
    return row * GRID_LENGTH + col;
}

Vector2 GridToScreen(size_t index)
{
    size_t col = index % GRID_LENGTH;
    size_t row = index / GRID_LENGTH;
    return { float(col * TILE_WIDTH), float(row * TILE_HEIGHT) };
}

vector<size_t> OverlapTiles(Rectangle rectangle)
{
    if (rectangle.x < 0.0f) rectangle.x = 0.0f;
    if (rectangle.y < 0.0f) rectangle.y = 0.0f;
    if (rectangle.x + rectangle.width > SCREEN_WIDTH) rectangle.x = SCREEN_WIDTH - rectangle.width;
    if (rectangle.y + rectangle.height > SCREEN_HEIGHT) rectangle.y = SCREEN_HEIGHT - rectangle.height;

    const size_t colMin = rectangle.x / TILE_WIDTH;
    const size_t rowMin = rectangle.y / TILE_HEIGHT;
    const size_t colMax = (rectangle.x + rectangle.width) / TILE_WIDTH;
    const size_t rowMax = (rectangle.y + rectangle.height) / TILE_HEIGHT;

    vector<size_t> indices;
    indices.reserve((colMax - colMin) * (rowMax - rowMin));
    for (size_t row = rowMin; row < rowMax; row++)
    {
        for (size_t col = colMin; col < colMax; col++)
        {
            indices.push_back(row * GRID_LENGTH + col);
        }
    }
    return indices;
}

vector<size_t> VisibleTiles(Circle target, float detectionRadius,
    const Obstacles& obstacles, const vector<size_t>& tiles)
{
    vector<size_t> visibilityTiles;
    visibilityTiles.reserve(tiles.size());
    for (size_t i : tiles)
    {
        Vector2 tileCenter = GridToScreen(i) + Vector2{ TILE_WIDTH * 0.5f, TILE_HEIGHT * 0.5f };
        Vector2 tileEnd = tileCenter + Normalize(target.position - tileCenter) * detectionRadius;
        if (IsCircleVisible(tileCenter, tileEnd, target, obstacles)) visibilityTiles.push_back(i);
    }
    return visibilityTiles;
}

void SaveObstacles(const Obstacles& obstacles, const char* path)
{
    ofstream file(path, ios::out | ios::trunc);
    for (const Circle& obstacle : obstacles)
        file << obstacle.position.x << " " << obstacle.position.y << " " << obstacle.radius << endl;
    file.close();
}

Obstacles LoadObstacles(const char* path)
{
    Obstacles obstacles;
    ifstream file(path);
    while (!file.eof())
    {
        Circle obstacle;
        file >> obstacle.position.x >> obstacle.position.y >> obstacle.radius;
        obstacles.push_back(std::move(obstacle));
    }
    file.close();
    return obstacles;
}

void SavePoints(const Points& points, const char* path)
{
    ofstream file(path, ios::out | ios::trunc);
    for (const Vector2& point : points)
        file << point.x << " " << point.y << endl;
    file.close();
}

Points LoadPoints(const char* path)
{
    Points points;
    ifstream file(path);
    while (!file.eof())
    {
        Vector2 point;
        file >> point.x >> point.y;
        points.push_back(std::move(point));
    }
    file.close();
    return points;
}

bool IsCollision(Vector2 lineStart, Vector2 lineEnd, const Obstacles& obstacles)
{
    for (const Circle& obstacle : obstacles)
    {
        if (CheckCollisionLineCircle(lineStart, lineEnd, obstacle))
            return true;
    }
    return false;
}

bool ResolveCollisions(Vector2& position, float radius, const Obstacles& obstacles)
{
    for (const Circle& obstacle : obstacles)
    {
        Vector2 mtv;
        if (CheckCollisionCircles(obstacle, {position, radius}, mtv))
        {
            position = position + mtv;
            return true;
        }
    }
    return false;
}

Vector2 Avoid(const Rigidbody& rb, float probeLength, float dt, const Obstacles& obstacles)
{
    auto avoid = [&](float angle, Vector2& acc) -> bool
    {
        if (IsCollision(rb.pos, rb.pos + Rotate(Normalize(rb.vel), angle * DEG2RAD) * probeLength, obstacles))
        {
            const Vector2 vf = Rotate(Normalize(rb.vel), rb.angularSpeed * dt * Sign(-angle)) * Length(rb.vel);
            acc = Acceleration(rb.vel, vf, dt);
            return true;
        }
        return false;
    };

    Vector2 acc{};
    if (avoid(-15.0f, acc)) return acc;
    if (avoid( 15.0f, acc)) return acc;
    if (avoid(-30.0f, acc)) return acc;
    if (avoid( 30.0f, acc)) return acc;
    return acc;
}

Vector2 Patrol(const Points& points, const Rigidbody& rb, size_t& index, float maxSpeed, float slowRadius, float pointRadius)
{
    index = Distance(rb.pos, points[index]) <= pointRadius ? ++index % points.size() : index;
    return Arrive(points[index], rb, maxSpeed, slowRadius);
}

// Sooooooo much less code thant the decision tree...
// Perhaps give this to students and ask them to implement this as a decision tree for future assignment 3?
/*
void Melee(Rigidbody& enemy, EnemyData& enemyData, const Rigidbody& player, const PlayerData& playerData,
    const Points& points, const Obstacles& obstacles)
{
    const Circle playerCircle{ player.pos, playerData.radius };

    // Player detected?
    if (CheckCollisionCircles({ enemy.pos, enemyData.sightDistance }, playerCircle))
    {
        // Player visible? (No FoV check or rotate till in FoV because this is complicated enough already)...
        if (IsCircleVisible(enemy.pos, enemy.pos + Normalize(player.pos - enemy.pos) * enemyData.sightDistance,
            playerCircle, obstacles))
        {
            // Within combat distance?
            if (CheckCollisionCircles({ enemy.pos, enemyData.combatDistance }, playerCircle))
            {
                // Close attack (must still call arrive)
                enemy.acc = Arrive(player.pos, enemy, enemyData.speed, 100.0f, 5.0f);
                printf("Melee attacking player\n");
            }
            else
            {
                // Seek player
                enemy.acc = Arrive(player.pos, enemy, enemyData.speed, 100.0f, 5.0f);
                printf("Melee seeking player\n");
            }
        }
        else
        {
            // Seek nearest visible tile
            Rectangle area = From({ enemy.pos, enemyData.radius });
            vector<size_t> overlap = OverlapTiles(area);
            vector<size_t> visible = VisibleTiles(playerCircle, enemyData.sightDistance, obstacles, overlap);
            Points points(visible.size());
            for (size_t i = 0; i < points.size(); i++)
                points[i] = GridToScreen(visible[i]);
            if (!points.empty())
                enemy.acc = Seek(NearestPoint(enemy.pos, points), enemy, enemyData.speed);
            printf("Melee seeking visibility\n");
        }
    }
    else
    {
        enemy.acc = Patrol(points, enemy, enemyData.point, enemyData.speed, 200.0f, 100.0f);
        printf("Melee patrolling\n");
    }
}

void Ranged(Rigidbody& enemy, EnemyData& enemyData, const Rigidbody& player, const PlayerData& playerData,
    const Points& points, const Obstacles& obstacles)
{

}
*/