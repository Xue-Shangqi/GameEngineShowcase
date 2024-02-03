#include "raylib-cpp.hpp"

int main(){
    int winWidth = 800;
    int winWidthResized = 800;
    int winHeight = 450;
    int winHeightResized = 450;
    std::string content = "What is 1 + 1?";

    raylib::Text text;

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
            text.Draw(content, winWidthResized/2 - textOffset, winHeightResized/2, 20, raylib::Color::Black());
            //textColor.DrawText("Congrats! You created your first window!", 190, 200, 20);
        }
        EndDrawing();
    }

}