#include "raylib-cpp.hpp"


raylib::Color LerpColor(raylib::Color start, raylib::Color end, double t) {
    t = fminf(1, fmaxf(0, t));
    double r = start.r + t * (end.r - start.r);
    double g = start.g + t * (end.g - start.g);
    double b = start.b + t * (end.b - start.b);
    return raylib::Color(r, g, b);
}


int main(){
    int winWidth = 800;
    int winWidthResized = 800;
    int winHeight = 450;
    int winHeightResized = 450;
    int colorIndexStart = 0;
    int colorIndexEnd = 1;
    double timeTracker = 0;
    double timeElapsed = 0;
    double lerpTime = 2;
    std::string content = "What is 1 + 1?";

    raylib::Color colorList[4] = {
        raylib::Color(185,255,248), 
        raylib::Color(111,237,214), 
        raylib::Color(255,149,81), 
        raylib::Color(255,74,74)
    };

    raylib::Text text;

    SetTargetFPS(60);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(winWidth, winHeight, "CS381 - Assignment 0");
    timeTracker = GetTime();
    while (!WindowShouldClose()) {
        BeginDrawing();
        {
			ClearBackground(RAYWHITE);
            if(IsWindowResized()){
                winWidthResized = GetScreenWidth();
                winHeightResized = GetScreenHeight();
            }
            int textOffset = (MeasureText(content.c_str(), 20))/2;

            //color changes every second base on the frame  
            timeElapsed += GetFrameTime();
            double t = timeElapsed / lerpTime;

            timeTracker += GetFrameTime();
            if(timeTracker >= lerpTime){
                colorIndexStart = (colorIndexStart+1) % 4;
                colorIndexEnd = (colorIndexEnd+1) % 4;
                timeTracker = 0;
                timeElapsed = 0;
            }    
            
            raylib::Color initColor = LerpColor(colorList[colorIndexStart], colorList[colorIndexEnd], t);
            raylib::Color finalColor = LerpColor(initColor, colorList[colorIndexEnd], t);
            text.Draw(content, winWidthResized/2 - textOffset, winHeightResized/2, 20, finalColor);

            
        }
        EndDrawing();
    }
}