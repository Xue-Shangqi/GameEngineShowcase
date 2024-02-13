#include "raylib-cpp.hpp"

//Function to draw and translate the models
void DrawBoundedModel(raylib::Model& model, Vector3 position, Vector3 scale, Vector3 rotation){
    raylib::Transform ogTransform = model.transform;
    model.transform = raylib::Transform(model.transform).Translate(position.x,position.y,position.z);
    model.transform = raylib::Transform(model.transform).Scale(scale.x, scale.y, scale.z);
    model.transform = raylib::Transform(model.transform).RotateXYZ(raylib::Degree(rotation.x), raylib::Degree(rotation.y), raylib::Degree(rotation.z));
    model.Draw({});
    model.GetTransformedBoundingBox().Draw();
    model.transform = ogTransform;
}

int main(){
    //Initialize window 
    SetTargetFPS(60);
    InitWindow(800, 450, "CS381 - Assignment 1");

    //Load ship
    raylib::Model ship = LoadModel("customModel/ddg51.glb");
    ship.transform = raylib::Transform(ship.transform).RotateXYZ(raylib::Degree(90), 0, raylib::Degree(90));

    //Load plane
    raylib::Model plane = LoadModel("customModel/PolyPlane.glb");
    plane.transform = raylib::Transform(plane.transform).Scale(3, 3, 3);

    //load bigBird 
    raylib::Model phoenix = LoadModel("customModel/phoenix_bird.glb");

    //load skeleton 
    raylib::Model head = LoadModel("customModel/head.glb");
    head.transform = raylib::Transform(head.transform).RotateXYZ(raylib::Degree(90), raylib::Degree(180), 0);


    raylib::Camera camera(
        raylib::Vector3(0, 100, 800),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );

    //Plane animation variables
    float planeOnePos = 100;
    float planeOneRot = 0;
    float planeTwoPos = -100;
    float planeTwoRot = 0;

    //Ship animation variables
    float shipOneRot = 0;
    float shipTwoRot = 90;
    float shipThreeRot = 270;

    //Head animation variables
    float headRot = 0;

    //Phoenix animation variables
    float birdRot = 0;

    while (!WindowShouldClose()) {
        //Plane animation 
        planeOnePos += 50 * GetFrameTime();
        planeOneRot += 30 * GetFrameTime();
        planeTwoPos -= 50 * GetFrameTime();
        planeTwoRot += 30 * GetFrameTime();

        //Ship animation  
        shipOneRot += 40 * GetFrameTime();
        shipTwoRot -= 40 * GetFrameTime();
        shipThreeRot += 40 * GetFrameTime();

        //Head animation
        headRot += 70 * GetFrameTime();

        //Phoenix animation
        birdRot += 55 * GetFrameTime();

        BeginDrawing();
        {   
            camera.BeginMode();
            {  
                //Clears the blackground so old drawing dont stack
                ClearBackground(BLACK);

                //Draws the models
                DrawBoundedModel(plane, {planeOnePos, 0, 0}, {1, 1, 1}, {planeOneRot, 0, 0});
                DrawBoundedModel(plane, {planeTwoPos, 100, 0}, {1, -1, 1}, {planeTwoRot, 180, 0});

                DrawBoundedModel(ship, {-350, 0, 0}, {1, 1, 1}, {0, shipOneRot, 0});
                DrawBoundedModel(ship, {350, 0, 0}, {1, 1, 1}, {0, shipTwoRot, 0});
                DrawBoundedModel(ship, {150, 150, 0}, {1, 2, 1}, {0, shipThreeRot, 0});
                DrawBoundedModel(phoenix, {0, -200, 0}, {.2, .2, .2}, {0, birdRot, 0});
                DrawBoundedModel(head, {0, -50, 0}, {50, 50, 50}, {0, headRot, 0});
            }
            camera.EndMode(); 
        }
        EndDrawing();
    }
    //Unload all model in the scene 
    UnloadModel(plane); 
    UnloadModel(ship); 
    UnloadModel(phoenix); 
    UnloadModel(head);
}