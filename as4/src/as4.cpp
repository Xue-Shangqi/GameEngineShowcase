#include "raylib-cpp.hpp"
#include "rlgl.h"

//lambda function for translation
auto translateModel = [](raylib::Model& model, float x, float y, float z){
    model.transform = raylib::Transform(model.transform).Translate(x,y,z);
};

//Function to draw and translate the models
void DrawModel(raylib::Model& model, Vector3 position, Vector3 scale, Vector3 rotation){
    raylib::Transform ogTransform = model.transform;
    translateModel(model, position.x, position.y, position.z);
    model.transform = raylib::Transform(model.transform).Scale(scale.x, scale.y, scale.z);
    model.transform = raylib::Transform(model.transform).RotateXYZ(raylib::Degree(rotation.x), raylib::Degree(rotation.y), raylib::Degree(rotation.z));
    model.Draw({});
    model.transform = ogTransform;
}

//Spaceship data
struct Spaceship{
    //Variables for position and speed 
    float speed = 1;
    raylib::Degree tiltTarget = 0;
    raylib::Degree tilt = 0;
    const float acceleration = 10.0f;
    const float deceleration = 0.1f;
    raylib::Vector3 velocity = {0, 0, 0};      
    raylib::Vector3 pos = {0, 0, 0};      
};

//Laser data
struct Laser{
    raylib::Vector3 pos = {0, 0, 0};
    float speed = 200;
    bool useable = true;
};

//Meteor data
struct Meteor{
    raylib::Vector3 pos = {0, 0, 0};
    float speed = 100; 
    bool exist = false;
};

//Function for player movement
void movementControl(Spaceship& spaceship){
    float maxSpeed = 3.0f;
    float tiltSpeed = 80.0f;

    //Movement conditions
    //Backward
    if (IsKeyDown(KEY_S)){
        spaceship.velocity.y -= spaceship.acceleration * GetFrameTime();
    }
    
    if(IsKeyReleased(KEY_S)){
        while(spaceship.velocity.y < 0){
            spaceship.velocity.y += spaceship.deceleration * GetFrameTime();
        }
    }

    //Forward
    if (IsKeyDown(KEY_W)){ 
        spaceship.velocity.y += spaceship.acceleration * GetFrameTime();
    }

    if(IsKeyReleased(KEY_W)){
        while(spaceship.velocity.y > 0){
            spaceship.velocity.y -= spaceship.deceleration * GetFrameTime();
        }
    }

    //Right
    if (IsKeyDown(KEY_A)){
        spaceship.velocity.x -= spaceship.acceleration * GetFrameTime();
        spaceship.tiltTarget = -30;
    }

    if(IsKeyReleased(KEY_A)){
        spaceship.tiltTarget = 0;
        while(spaceship.velocity.x < 0){
            spaceship.velocity.x += spaceship.deceleration * GetFrameTime();
        }
    }

    //Left
    if (IsKeyDown(KEY_D)){
        spaceship.velocity.x += spaceship.acceleration * GetFrameTime();
        spaceship.tiltTarget = 30;
    }

    if(IsKeyReleased(KEY_D)){
        spaceship.tiltTarget = 0;
        while(spaceship.velocity.x > 0){
            spaceship.velocity.x -= spaceship.deceleration * GetFrameTime();
        }
    }

    //Limit speed to max speed
    spaceship.velocity.x = (spaceship.velocity.x > maxSpeed) ? maxSpeed : ((spaceship.velocity.x < -maxSpeed) ? -maxSpeed : spaceship.velocity.x);
    spaceship.velocity.y = (spaceship.velocity.y > maxSpeed) ? maxSpeed : ((spaceship.velocity.y < -maxSpeed) ? -maxSpeed : spaceship.velocity.y);

    //Updates the position of ship
    spaceship.pos.x += spaceship.velocity.x;
    spaceship.pos.y += spaceship.velocity.y;

    //Updates the tilt of ship
    if(spaceship.tilt < spaceship.tiltTarget){
        spaceship.tilt += tiltSpeed * GetFrameTime();
    }
    if(spaceship.tilt > spaceship.tiltTarget){
        spaceship.tilt -= tiltSpeed * GetFrameTime();
    }


    //Warps the player around screen
    if (spaceship.pos.x > 220) {
        spaceship.pos.x = -220;
    } else if (spaceship.pos.x < -220) {
        spaceship.pos.x = 220;
    }

    if (spaceship.pos.y > 215) {
        spaceship.pos.y = -35;
    } else if (spaceship.pos.y < -35) {
        spaceship.pos.y = 215;
    }
}

void shootingControl(const Spaceship spaceship, Laser& laser, int& curLaser) {
    if(IsKeyPressed(KEY_SPACE)){
        if(laser.useable == true){
            laser.pos = spaceship.pos;
            laser.useable = false;
        }
        curLaser++;
        curLaser %= 10;
    }
}

void laserCalc(Laser& laser, raylib::Model& cube){
    if(!laser.useable && laser.pos.y < 200){
        laser.pos.y += laser.speed * GetFrameTime();
        DrawModel(cube, {laser.pos.x, laser.pos.y + 10, laser.pos.z}, {1, 1, 1}, {0, 0, 0});
    }else if(laser.pos.y > 30){
        laser.useable = true;
    }
}

void spawnMeteor(Meteor& meteor){
    if(!meteor.exist){
        int randomX = GetRandomValue(-1, 1);
        meteor.pos.x = randomX;
        meteor.exist = true;
    }
}

void meteorCalc(Meteor& meteor, raylib::Model& model){
    if(meteor.exist && meteor.pos.y > -200){
        meteor.pos.y -= meteor.speed * GetFrameTime();
        DrawModel(model, meteor.pos, {1, 1, 1}, {0});
    }else if(meteor.pos.y < -200){
        meteor.exist = false;
    }
}

int main(){
    //Initialize window 
    int screenWidth = 800;
    int screenHeight = 450;
    SetTargetFPS(60);
    InitWindow(screenWidth, screenHeight, "CS381 - Assignment 4");
    
    //Loads spaceship model
    raylib::Model spaceship = LoadModel("customModel/spaceship.glb");
    spaceship.transform = raylib::Transform(spaceship.transform).Scale(2, 2, 2);
    spaceship.transform = raylib::Transform(spaceship.transform).RotateXYZ(raylib::Degree(90), raylib::Degree(180), 0);
    translateModel(spaceship, 0, -(screenHeight/4.3), -200);
    Spaceship spaceshipOne;

    //Init lasers
    int curLaser = 0;
    raylib::Model cube = LoadModelFromMesh(GenMeshCube(1, 5, 1));
    translateModel(cube, 0, -(screenHeight/4.3), -200);
    Laser laserOne;
    Laser laserTwo;
    Laser laserThree;
    Laser laserFour;
    Laser laserFive;
    Laser laserSix;
    Laser laserSeven;
    Laser laserEight;
    Laser laserNine;
    Laser laserTen;

    //Init meters 
    raylib::Model meteor = LoadModel("customModel/meteor.glb");
    meteor.transform = raylib::Transform(meteor.transform).Scale(2.2, 2.2, 2.2);
    translateModel(meteor, 0, 40, 0);
    Meteor meteorOne;
    Meteor meteorTwo;
    Meteor meteorThree;
    Meteor meteorFour;
    Meteor meteorFive;
    Meteor meteorSix;
    Meteor meteorSeven;
    Meteor meteorEight;
    Meteor meteorNine;
    Meteor meteorTen;

    //Load background
    Texture2D background = LoadTexture("textures/space.png");
    float scrollingBack = 0.0f;

    //Set up camera
    raylib::Camera camera(
        raylib::Vector3(0, 0, 100),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );

    //Set up sound
    InitAudioDevice();
    Sound engine = LoadSound("sound/engineSound.wav");
    bool isspaceshipMoving = false;
    

    while (!WindowShouldClose()) {
        //Engine sound
        if ((spaceshipOne.speed != 0)) {
            isspaceshipMoving = true;
            //Play the sound if it's not already playing
            if (!IsSoundPlaying(engine)) {
                PlaySound(engine);
            }
        } else {
            isspaceshipMoving = false;
            //Stop the sound if it's playing
            if (IsSoundPlaying(engine)) {
                StopSound(engine);
            }
        }

        //Scrolling background control
        scrollingBack += 1.5f;
        if (scrollingBack >= background.height * 2) scrollingBack = 0;


        BeginDrawing();
        {   
            //Set up for drawing
            ClearBackground(BLACK);
            DrawTextureEx(background, (Vector2){0, scrollingBack}, 0.0f, 2.0f, WHITE);
            DrawTextureEx(background, (Vector2){0, -background.height * 2 + scrollingBack}, 0.0f, 2.0f, WHITE);

            //Camera starts
            camera.BeginMode();
            {
        
                //Draws the model
                DrawModel(spaceship, spaceshipOne.pos, {1, 1, 1}, {0, spaceshipOne.tilt, 0});

                //Basic controls 
                movementControl(spaceshipOne);

                //Shooting control, it was a pain to figure out without list Q_Q
                switch(curLaser){
                    case 1:
                        shootingControl(spaceshipOne, laserTwo, curLaser);
                        break;
                    case 2:
                        shootingControl(spaceshipOne, laserThree, curLaser);
                        break;
                    case 3:
                        shootingControl(spaceshipOne, laserFour, curLaser);
                        break;
                    case 4:
                        shootingControl(spaceshipOne, laserFive, curLaser);
                        break;
                    case 5:
                        shootingControl(spaceshipOne, laserSix, curLaser);
                        break;
                    case 6:
                        shootingControl(spaceshipOne, laserSeven, curLaser);
                        break;
                    case 7:
                        shootingControl(spaceshipOne, laserEight, curLaser);  
                        break;
                    case 8:
                        shootingControl(spaceshipOne, laserNine, curLaser);
                        break;
                    case 9:
                        shootingControl(spaceshipOne, laserTen, curLaser);
                        break;
                    default:
                        shootingControl(spaceshipOne, laserOne, curLaser);
                        break;
                }
                laserCalc(laserOne, cube);
                laserCalc(laserTwo, cube);
                laserCalc(laserThree, cube);
                laserCalc(laserFour, cube);
                laserCalc(laserFive, cube);
                laserCalc(laserSix, cube);
                laserCalc(laserSeven, cube);
                laserCalc(laserEight, cube);
                laserCalc(laserNine, cube);
                laserCalc(laserTen, cube);

                //Spawn random meteors at top of the screen
                spawnMeteor(meteorOne);
                spawnMeteor(meteorTwo);
                spawnMeteor(meteorThree);
                spawnMeteor(meteorFour);
                spawnMeteor(meteorFive);
                spawnMeteor(meteorSix);
                spawnMeteor(meteorSeven);
                spawnMeteor(meteorEight);
                spawnMeteor(meteorNine);
                spawnMeteor(meteorTen);
                meteorCalc(meteorOne, meteor);
                meteorCalc(meteorTwo, meteor);
                meteorCalc(meteorThree, meteor);
                meteorCalc(meteorFour, meteor);
                meteorCalc(meteorFive, meteor);
                meteorCalc(meteorSix, meteor);
                meteorCalc(meteorSeven, meteor);
                meteorCalc(meteorEight, meteor);
                meteorCalc(meteorNine, meteor);
                meteorCalc(meteorTen, meteor);

            }
            //Camera ends
            camera.EndMode(); 

            //Used for testing 
            raylib::Text text;
            text.Draw("X: " + std::to_string(spaceshipOne.pos.x), 10, 30, 20, raylib::Color::RayWhite());
            text.Draw("Y: " + std::to_string(spaceshipOne.pos.y), 10, 10, 20, raylib::Color::RayWhite());
        }
        EndDrawing();
    }

    //Unload and closes program
    UnloadModel(spaceship);
    UnloadSound(engine);
    CloseAudioDevice();
}