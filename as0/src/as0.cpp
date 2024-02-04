#include "raylib-cpp.hpp"


raylib::Color LerpColor(raylib::Color start, raylib::Color end, double t) {
    t = fminf(1, fmaxf(0, t));
    double r = start.r + t * (end.r - start.r);
    double g = start.g + t * (end.g - start.g);
    double b = start.b + t * (end.b - start.b);
    return raylib::Color(r, g, b);
}

int LerpSize(int start, int end, double t){
    t = fminf(1, fmaxf(0, t));
    int size = start + t *(end - start);
    return size; 
}

int main(){
    int winWidth = 800;
    int winWidthResized = 800;
    int winHeight = 450;
    int winHeightResized = 450;
    int colorIndex = 0;
    int size = 50;
    double timeElapsed = 0;
    double lerpTime = 3;
    std::string content = "What is 1 + 1?";

    raylib::Color colorList[4] = {
        raylib::Color(185,255,248), 
        raylib::Color(111,237,214), 
        raylib::Color(255,149,81), 
        raylib::Color(255,74,74)
    };

    raylib::Text text;

    SetTargetFPS(120);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(winWidth, winHeight, "CS381 - Assignment 0");
    while (!WindowShouldClose()) {
        BeginDrawing();
        {

			ClearBackground(BLACK);
            if(IsWindowResized()){
                winWidthResized = GetScreenWidth();
                winHeightResized = GetScreenHeight();
            }

            timeElapsed += GetFrameTime();
            double t = timeElapsed / lerpTime;

            if(timeElapsed >= lerpTime){
                colorIndex = (colorIndex + 1) % 4;
                timeElapsed = 0;
            }    

            
            int textOffset = (MeasureText(content.c_str(), size))/2;

            raylib::Color finalColor = LerpColor(colorList[colorIndex], colorList[(colorIndex + 1) % 4], t); 
            size = LerpSize(20, 70, t);
            
            text.Draw(content, winWidthResized/2 - textOffset, winHeightResized/2, size, finalColor);
            text.Draw("FPS: " + std::to_string(GetFPS()), 10, 10, 20, raylib::Color::RayWhite());
            
        }
        EndDrawing();
    }
    return 0;
}