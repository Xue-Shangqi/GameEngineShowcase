#include "raylib-cpp.hpp"

auto translateModel = [](raylib::Model& model, float x, float y, float z){
    model.transform = raylib::Transform(model.transform).Translate(x,y,z);
};

void setUpModel(
    raylib::Model& model, 
    float x, float y, float z, 
    float scaleX, float scaleY, float scaleZ, 
    float degreeX, float degreeY, float degreeZ){
    translateModel(model, x, y ,z);
    model.transform = raylib::Transform(model.transform).Scale(scaleX, scaleY, scaleZ);
    model.transform = raylib::Transform(model.transform).RotateXYZ(raylib::Degree(degreeX), raylib::Degree(degreeY), raylib::Degree(degreeZ));
}


int main(){
    //Initialize window 
    SetTargetFPS(60);
    InitWindow(800, 450, "CS381 - Assignment 1");

    //Setup poly plane default
    raylib::Model planeDefault = LoadModel("../../381Resources/meshes/PolyPlane.glb");
    setUpModel(planeDefault, 0, 0, 0, 3, 3, 3, 0, 0, 0);

    //Setup poly plane with changes
    raylib::Model planeChanged = LoadModel("../../381Resources/meshes/PolyPlane.glb");
    setUpModel(planeChanged, -100, 100, 0, 3, -3, 3, 0, 180, 0);

    //Setup ship one (left ship) 
    raylib::Model shipOne = LoadModel("../../381Resources/meshes/ddg51.glb");
    setUpModel(shipOne, -200, 0, 0, 1, 1, 1, 90, 0, 90);

    //Setup ship two (rightmost ship) 
    raylib::Model shipTwo = LoadModel("../../381Resources/meshes/ddg51.glb");
    setUpModel(shipTwo, 200, 0, 0, 1, 1, 1, 90, 0, 0);

    //Setup ship two (middle-ish ship) 
    raylib::Model shipThree = LoadModel("../../381Resources/meshes/ddg51.glb");
    setUpModel(shipThree, 100, 100, 0, 1, 2, 1, 270, 180, 0);

    //Setup bigBird 
    raylib::Model phoenix = LoadModel("../customModel/phoenix_bird.glb");
    setUpModel(phoenix, 100, -100, 0, .2, .2, .2, 0, 0, 0);

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
                DrawModel(planeDefault, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(planeChanged, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(shipOne, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(shipTwo, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(shipThree, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(phoenix, raylib::Vector3(0,0,0), 1, WHITE);
                DrawBoundingBox(planeDefault.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(planeChanged.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(shipOne.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(shipTwo.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(shipThree.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(phoenix.GetTransformedBoundingBox(), WHITE);
            }
            camera.EndMode();
        }
        EndDrawing();
    }

    //Unload all model in the scene 
    UnloadModel(planeDefault); 
    UnloadModel(planeChanged); 
    UnloadModel(shipOne); 
    UnloadModel(shipTwo); 
    UnloadModel(shipThree); 
    UnloadModel(phoenix); 
}