#include "raylib-cpp.hpp"

int main(){
    int winWidth = 800;
    int winWidthResized = 800;
    int winHeight = 450;
    int winHeightResized = 450;
    int colorIndex = 0;
    std::string content = "What is 1 + 1?";

    raylib::Color colorList[4] = {raylib::Color(185,255,248), raylib::Color(111,237,214), raylib::Color(255,149,81), raylib::Color(255,74,74)};
    raylib::Text text;

    SetTargetFPS(1);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(winWidth, winHeight, "CS381 - Assignment 0");
    while (!WindowShouldClose()) {
        BeginDrawing();
        {
			ClearBackground(RAYWHITE);
            if(IsWindowResized()){
                winWidthResized = GetScreenWidth();
                winHeightResized = GetScreenHeight();
            }
            int textOffset = (MeasureText(content.c_str(), 20))/2;
            text.Draw(content, winWidthResized/2 - textOffset, winHeightResized/2, 20, colorList[colorIndex]);
            if(GetFrameTime() >= 1){
                colorIndex = ((colorIndex+1) % 4);
            }
        }
        EndDrawing();
    }

}