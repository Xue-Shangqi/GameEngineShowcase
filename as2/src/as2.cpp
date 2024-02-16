#include "raylib-cpp.hpp"

//lambda function for translation
auto translateModel = [](raylib::Model& model, float x, float y, float z){
    model.transform = raylib::Transform(model.transform).Translate(x,y,z);
};

//Function to draw and translate the models
void DrawBoundedModel(raylib::Model& model, Vector3 position, Vector3 scale, Vector3 rotation){
    raylib::Transform ogTransform = model.transform;
    translateModel(model, position.x, position.y, position.z);
    model.transform = raylib::Transform(model.transform).Scale(scale.x, scale.y, scale.z);
    model.transform = raylib::Transform(model.transform).RotateXYZ(raylib::Degree(rotation.x), raylib::Degree(rotation.y), raylib::Degree(rotation.z));
    model.Draw({});
    model.GetTransformedBoundingBox().Draw();
    model.transform = ogTransform;
}

int main(){
    //Initialize window 
    SetTargetFPS(60);
    InitWindow(800, 450, "CS381 - Assignment 2");
    
    //Loads plane model
    raylib::Model plane = LoadModel("customModel/PolyPlane.glb");
    plane.transform = raylib::Transform(plane.transform).Scale(3, 3, 3);
    plane.transform = raylib::Transform(plane.transform).RotateXYZ(0, raylib::Degree(180), 0);

    //Set up camera
    raylib::Camera camera(
        raylib::Vector3(0, 100, 800),
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
                DrawBoundedModel(plane, {0, 0, 0}, {1, 1, 1}, {0, 0, 0});
            }
            camera.EndMode(); 
        }
        EndDrawing();
    }
}