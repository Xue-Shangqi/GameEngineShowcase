#include "raylib-cpp.hpp"

//lambda function for translation
auto translateModel = [](raylib::Model& model, float x, float y, float z){
    model.transform = raylib::Transform(model.transform).Translate(x,y,z);
};

//function that is used to initialize and setup all the models so code looks cleaner
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
    raylib::Model planeDefault = LoadModel("../customModel/PolyPlane.glb");
    setUpModel(planeDefault, 100, 0, 0, 3, 3, 3, 0, 0, 0);

    //Setup poly plane with changes
    raylib::Model planeChanged = LoadModel("../customModel/PolyPlane.glb");
    setUpModel(planeChanged, -100, 100, 0, 3, -3, 3, 0, 180, 0);

    //Setup ship one (left ship) 
    raylib::Model shipOne = LoadModel("../customModel/ddg51.glb");
    setUpModel(shipOne, -350, 0, 0, 1, 1, 1, 90, 0, 90);

    //Setup ship two (rightmost ship) 
    raylib::Model shipTwo = LoadModel("../customModel/ddg51.glb");
    setUpModel(shipTwo, 350, 0, 0, 1, 1, 1, 90, 0, 0);

    //Setup ship two (middle-ish ship) 
    raylib::Model shipThree = LoadModel("../customModel/ddg51.glb");
    setUpModel(shipThree, 150, 150, 0, 1, 2, 1, 270, 180, 0);

    //Setup bigBird 
    raylib::Model phoenix = LoadModel("../customModel/phoenix_bird.glb");
    setUpModel(phoenix, 0, -200, 0, .2, .2, .2, 0, 0, 0);

    //Setup skeleton 
    raylib::Model head = LoadModel("../customModel/head.glb");
    setUpModel(head, 0, -50, 0, 50, 50, 50, -90, 0, 0);

    raylib::Camera camera(
        raylib::Vector3(0, 100, 800),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );

    float rotation = 0.0f;

    while (!WindowShouldClose()) {
        BeginDrawing();
        {   
            camera.BeginMode();
            {   
                //Animation for models
                planeDefault.transform = raylib::Transform(planeDefault.transform).RotateXYZ(2*GetFrameTime(),0,0);
                translateModel(planeDefault, 2, 0, 0);
                planeChanged.transform = raylib::Transform(planeChanged.transform).RotateXYZ(-2*GetFrameTime(),0,0);
                translateModel(planeChanged, -2, 0, 0);
                shipOne.transform = raylib::Transform(shipOne.transform).RotateXYZ(0,2*GetFrameTime(),0);
                shipTwo.transform = raylib::Transform(shipTwo.transform).RotateXYZ(0,-2*GetFrameTime(),0);
                shipThree.transform = raylib::Transform(shipThree.transform).RotateXYZ(0,2*GetFrameTime(),0);
                phoenix.transform = raylib::Transform(phoenix.transform).RotateXYZ(0,2*GetFrameTime(),0);
                head.transform = raylib::Transform(head.transform).RotateXYZ(0,2*GetFrameTime(),0);

                //Clears the blackground so old drawing dont stack
                ClearBackground(BLACK);

                //Draws the models
                DrawModel(planeDefault, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(planeChanged, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(shipOne, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(shipTwo, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(shipThree, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(phoenix, raylib::Vector3(0,0,0), 1, WHITE);
                DrawModel(head, raylib::Vector3(0,0,0), 1, WHITE);

                //Draws the bounding boxes
                DrawBoundingBox(planeDefault.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(planeChanged.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(shipOne.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(shipTwo.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(shipThree.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(phoenix.GetTransformedBoundingBox(), WHITE);
                DrawBoundingBox(head.GetTransformedBoundingBox(), WHITE);

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
    UnloadModel(head);
}