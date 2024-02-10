#include "raylib-cpp.hpp"

auto translateModel = [](raylib::Model& model, float x, float y, float z){
    model.transform = raylib::Transform(model.transform).Translate(x,y,z);
};

int main(){
    //Initialize window 
    SetTargetFPS(60);
    InitWindow(800, 450, "CS381 - Assignment 1");

    //Setup poly plane 
    raylib::Model planeOne = LoadModel("../../381Resources/meshes/PolyPlane.glb");
    planeOne.transform = raylib::Transform(planeOne.transform).Scale(1,-1,1);
    planeOne.transform = raylib::Transform(planeOne.transform).RotateXYZ(0, raylib::Degree(180), 0);
    translateModel(planeOne, -100, 100 ,0);

    
    raylib::Camera camera(
        raylib::Vector3(0, 100, 500),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            camera.BeginMode();
            {
                DrawModel(planeOne, raylib::Vector3(0,0,0), 1, WHITE);
                DrawBoundingBox(GetModelBoundingBox(planeOne), WHITE);
            }
            camera.EndMode();
        }
        EndDrawing();
    }

    UnloadModel(planeOne); 
}