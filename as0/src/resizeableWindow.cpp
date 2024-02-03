#include "raylib-cpp.hpp"

int main(){
    int winWidth = 800;
    int winHeight = 450;

    raylib::Window window(winWidth, winHeight, "CS381 - Assignment 0");
    raylib::Text text;

    while (!window.ShouldClose()) {
        window.BeginDrawing();
		{
			window.ClearBackground(RAYWHITE);
            text.Draw("What is 1 + 1", 0, winWidth/2, winHeight/2, raylib::Color::Black());

            //textColor.DrawText("Congrats! You created your first window!", 190, 200, 20);

        }
        window.EndDrawing();
    }

}