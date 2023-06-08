#include "rlImGui.h"
#include "Physics.h"
#include "Collision.h"

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

int main(void)
{
   
            // Render GUI
        if (IsKeyPressed(KEY_GRAVE)) demoGUI = !demoGUI;
        if (demoGUI)
        {
            rlImGuiBegin();
            ImGui::ShowDemoWindow(nullptr);
            rlImGuiEnd();
        }

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
