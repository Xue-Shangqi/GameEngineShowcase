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
    raylib::Vector2 dimensions;
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
    float speed = 20; 
    bool exist = false;
};

//Function for player movement
void movementControl(Spaceship& spaceship){
    float maxSpeed = 1.5f;
    float tiltSpeed = 80.0f;

    if(IsKeyDown(KEY_LEFT_CONTROL)){
        maxSpeed = 5.0f;
        tiltSpeed = 266.66f;
    }

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
        spaceship.velocity.x += spaceship.acceleration * GetFrameTime();
        spaceship.tiltTarget = -30;
    }

    if(IsKeyReleased(KEY_A)){
        spaceship.tiltTarget = 0;
        while(spaceship.velocity.x > 0){
            spaceship.velocity.x -= spaceship.deceleration * GetFrameTime();
        }
    }

    //Left
    if (IsKeyDown(KEY_D)){
        spaceship.velocity.x -= spaceship.acceleration * GetFrameTime();
        spaceship.tiltTarget = 30;
    }

    if(IsKeyReleased(KEY_D)){
        spaceship.tiltTarget = 0;
        while(spaceship.velocity.x < 0){
            spaceship.velocity.x += spaceship.deceleration * GetFrameTime();
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
    if (spaceship.pos.x > 80) {
        spaceship.pos.x = -80;
    } else if (spaceship.pos.x < -80) {
        spaceship.pos.x = 80;
    }

    if (spaceship.pos.y > 83) {
        spaceship.pos.y = -7;
    } else if (spaceship.pos.y < -7) {
        spaceship.pos.y = 83;
    }
}

//Set up the laser and plays laser sound
void shootingControl(Sound laserSound, const Spaceship spaceship, Laser& laser, int& curLaser) {
    if(IsKeyPressed(KEY_SPACE)){
        if(laser.useable == true){
            PlaySound(laserSound);
            laser.pos = spaceship.pos;
            laser.useable = false;
        }
        curLaser++;
        curLaser %= 10;
    }
}
//Calculations the position of laser and when to stop drawing
void laserCalc(Laser& laser, raylib::Model& cube){
    if(!laser.useable && laser.pos.y < 200){
        laser.pos.y += laser.speed * GetFrameTime();
        DrawModel(cube, {laser.pos.x, laser.pos.y + 10, laser.pos.z}, {1, 1, 1}, {0, 0, 0});
    }else if(laser.pos.y > 30){
        laser.useable = true;
    }
}

//Set up the meteor
void spawnMeteor(Meteor& meteor){
    if(!meteor.exist){
        int randomX = GetRandomValue(-70, 70);
        int randomY = GetRandomValue(0, 100);
        meteor.pos.x = randomX;
        meteor.pos.y = randomY;
        meteor.exist = true;
    }
}
//Calculations the position of meteor and when to stop drawing
void meteorCalc(Meteor& meteor, raylib::Model& model){
    if(meteor.exist && meteor.pos.y > -120){
        meteor.pos.y -= meteor.speed * GetFrameTime();
        DrawModel(model, {meteor.pos.x, meteor.pos.y, meteor.pos.z} , {1, 1, 1}, {0, 0, 0});
    }else if(meteor.pos.y < -120){
        meteor.pos.y = 0;
        meteor.exist = false;
    }
}

//Check if the bounding boxes of the laser and meteor intersect
void laserMeteorCollision(int& score, Laser& laser, Meteor& meteor, raylib::Camera camera) {
    Rectangle laserBox = {GetWorldToScreen(laser.pos, camera).x,  GetWorldToScreen(laser.pos, camera).y + 160, 2.0f, 15.0f};
    Rectangle meteorBox = {GetWorldToScreen(meteor.pos, camera).x - 40,  GetWorldToScreen(meteor.pos, camera).y - 350, 85.0f, 50.0f};

    if(!laser.useable && CheckCollisionRecs(laserBox, meteorBox)) {
        laser.useable = true;
        meteor.exist = false;
        score += 10;
    }
}

//Check if the bound boxes of player and meteor intersect
void meteorPlayerCollision(bool& alive, Spaceship& player, Meteor& meteor, raylib::Camera camera) {
    Rectangle spaceshipBox = {GetWorldToScreen(player.pos, camera).x - 25,  GetWorldToScreen(player.pos, camera).y + 165, 50.0f, 80.0f};
    Rectangle meteorBox = {GetWorldToScreen(meteor.pos, camera).x - 40,  GetWorldToScreen(meteor.pos, camera).y - 350, 85.0f, 50.0f};

    if(alive && CheckCollisionRecs(spaceshipBox, meteorBox)) {
        alive = false;
        meteor.exist = false;
    }
}

//Calculate center text 
Vector2 calcTextMid(std::string content, int font){
    int textWidth = MeasureText(content.c_str(), font);
    int textHeight = font;
    float posX = (800 - textWidth) / 2;
    float posY = (450 - textHeight) / 2;

    return Vector2{posX, posY};
}

//Enum for different screens
enum CurScreen {START = 0, PLAY, WIN, LOSE};


int main(){
    //Initialize window 
    int screenWidth = 800;
    int screenHeight = 450;
    SetTargetFPS(60);
    InitWindow(screenWidth, screenHeight, "CS381 - Assignment 4");
    int score = 0;
    CurScreen curScreen = START;

    //Init for text
    raylib::Text text;
    std::string title = "Very Scuffed Space Shooter Game";
    std::string startText = "Start";
    std::string winText = "You Win!!!";
    std::string winTextTwo = "why did you even spend time beating it lol";
    std::string loseText = "WOMP WOMP, L + Ratio";
    std::string restartText = "Restart?";
    Vector2 titlePos = calcTextMid(title, 30);
    Vector2 startPos = calcTextMid(startText, 20);
    Vector2 winOnePos = calcTextMid(winText, 40);
    Vector2 winTwoPos = calcTextMid(winTextTwo, 15);
    Vector2 losePos = calcTextMid(loseText, 30);
    Vector2 restartPos = calcTextMid(restartText, 20);

    //Init start button and mouse
    Vector2 mousePosition = GetMousePosition();
    Rectangle startButton = {(float)(screenWidth / 2 - 50), (float)(screenHeight / 2 + 20), 100, 50};

    //Loads spaceship model
    raylib::Model spaceship = LoadModel("customModel/spaceship.glb");
    spaceship.transform = raylib::Transform(spaceship.transform).RotateXYZ(raylib::Degree(90), raylib::Degree(180), raylib::Degree(180));
    translateModel(spaceship, 0, -40, 0);
    Spaceship spaceshipOne;
    bool alive = true;

    //Init lasers
    int curLaser = 0;
    raylib::Model cube = LoadModelFromMesh(GenMeshCube(1, 5, 1));
    cube.transform = raylib::Transform(cube.transform).Scale(0.5, 0.5, 0.5);
    translateModel(cube, 0, -40, 0);
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
    translateModel(meteor, 0, 60, 0);
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
        raylib::Vector3(0, 0, -100),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );

    //Set up sound
    InitAudioDevice();
    SetMasterVolume(0.3f);
    Sound laserSound = LoadSound("sound/laser.mp3");
    

    while (!WindowShouldClose()) {
        switch(curScreen){
            case START:
                    alive = true;
                    //Checks if the mouse clicks the start button
                    mousePosition = GetMousePosition();
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                        if (CheckCollisionPointRec(mousePosition, startButton) && curScreen == START) {
                            curScreen = PLAY;
                        }
                    }
                break;
            case PLAY:
                if(score >= 1000){
                    curScreen = WIN;
                }

                if(!alive){
                    curScreen = LOSE;
                }
                break;
            case WIN:
                break;
            case LOSE:
                    mousePosition = GetMousePosition();
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                        if (CheckCollisionPointRec(mousePosition, startButton) && curScreen == LOSE) {
                            curScreen = START;
                        }
                    }
                break;
            default: break;
        }



        BeginDrawing();
        {   
            //Scrolling background control
            scrollingBack += 1.5f;
            if (scrollingBack >= background.height * 2) scrollingBack = 0;

            //Set up for drawing
            ClearBackground(BLACK);
            DrawTextureEx(background, (Vector2){0, scrollingBack}, 0.0f, 2.0f, WHITE);
            DrawTextureEx(background, (Vector2){0, -background.height * 2 + scrollingBack}, 0.0f, 2.0f, WHITE);

            switch(curScreen){
                case START:
                    //Draws the title of game
                    DrawRectangle(screenWidth/2 - 50, screenHeight/2 + 20, 100, 50, GetColor(0x052c46ff));
                    text.Draw(title, titlePos.x, titlePos.y - 50, 30, PURPLE);
                    text.Draw(startText, startPos.x, startPos.y + 45, 20, RAYWHITE);

                    //Reset stage
                    score = 0;

                    spaceshipOne.pos = raylib::Vector3{0, 0, 0};
                    spaceshipOne.velocity = raylib::Vector3{0, 0, 0};
                    spaceshipOne.tilt = spaceshipOne.tiltTarget = 0;

                    meteorOne.exist = false;
                    meteorTwo.exist = false;
                    meteorThree.exist = false;
                    meteorFour.exist = false;
                    meteorFive.exist = false;
                    meteorSix.exist = false;
                    meteorSeven.exist = false;
                    meteorEight.exist = false;
                    meteorNine.exist = false;
                    meteorTen.exist = false;

                    laserOne.useable = true;
                    laserTwo.useable = true;
                    laserThree.useable = true;
                    laserFour.useable = true;
                    laserFive.useable = true;
                    laserSix.useable = true;
                    laserSeven.useable = true;
                    laserEight.useable = true;
                    laserNine.useable = true;
                    laserTen.useable = true;

                    //Camera starts
                    camera.BeginMode();
                    {
                        //Draws the model
                        DrawModel(spaceship, spaceshipOne.pos, {1, 1, 1}, {0, spaceshipOne.tilt, 0});
                    }
                    //Camera ends
                    camera.EndMode(); 

                    break;
                case PLAY:
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
                                shootingControl(laserSound, spaceshipOne, laserTwo, curLaser);
                                break;
                            case 2:
                                shootingControl(laserSound, spaceshipOne, laserThree, curLaser);
                                break;
                            case 3:
                                shootingControl(laserSound, spaceshipOne, laserFour, curLaser);
                                break;
                            case 4:
                                shootingControl(laserSound, spaceshipOne, laserFive, curLaser);
                                break;
                            case 5:
                                shootingControl(laserSound, spaceshipOne, laserSix, curLaser);
                                break;
                            case 6:
                                shootingControl(laserSound, spaceshipOne, laserSeven, curLaser);
                                break;
                            case 7:
                                shootingControl(laserSound, spaceshipOne, laserEight, curLaser);  
                                break;
                            case 8:
                                shootingControl(laserSound, spaceshipOne, laserNine, curLaser);
                                break;
                            case 9:
                                shootingControl(laserSound, spaceshipOne, laserTen, curLaser);
                                break;
                            default:
                                shootingControl(laserSound, spaceshipOne, laserOne, curLaser);
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


                        //---------------------------------------------------------------------
                        //WARNING: DO NOT LOOK BELOW 
                        //---------------------------------------------------------------------
                        //Collision checking
                        {
                        laserMeteorCollision(score, laserOne, meteorOne, camera);
                        laserMeteorCollision(score, laserOne, meteorTwo, camera);
                        laserMeteorCollision(score, laserOne, meteorThree, camera);
                        laserMeteorCollision(score, laserOne, meteorFour, camera);
                        laserMeteorCollision(score, laserOne, meteorFive, camera);
                        laserMeteorCollision(score, laserOne, meteorSix, camera);
                        laserMeteorCollision(score, laserOne, meteorSeven, camera);
                        laserMeteorCollision(score, laserOne, meteorEight, camera);
                        laserMeteorCollision(score, laserOne, meteorNine, camera);
                        laserMeteorCollision(score, laserOne, meteorTen, camera);

                        laserMeteorCollision(score, laserTwo, meteorOne, camera);
                        laserMeteorCollision(score, laserTwo, meteorTwo, camera);
                        laserMeteorCollision(score, laserTwo, meteorThree, camera);
                        laserMeteorCollision(score, laserTwo, meteorFour, camera);
                        laserMeteorCollision(score, laserTwo, meteorFive, camera);
                        laserMeteorCollision(score, laserTwo, meteorSix, camera);
                        laserMeteorCollision(score, laserTwo, meteorSeven, camera);
                        laserMeteorCollision(score, laserTwo, meteorEight, camera);
                        laserMeteorCollision(score, laserTwo, meteorNine, camera);
                        laserMeteorCollision(score, laserTwo, meteorTen, camera);

                        laserMeteorCollision(score, laserThree, meteorOne, camera);
                        laserMeteorCollision(score, laserThree, meteorTwo, camera);
                        laserMeteorCollision(score, laserThree, meteorThree, camera);
                        laserMeteorCollision(score, laserThree, meteorFour, camera);
                        laserMeteorCollision(score, laserThree, meteorFive, camera);
                        laserMeteorCollision(score, laserThree, meteorSix, camera);
                        laserMeteorCollision(score, laserThree, meteorSeven, camera);
                        laserMeteorCollision(score, laserThree, meteorEight, camera);
                        laserMeteorCollision(score, laserThree, meteorNine, camera);
                        laserMeteorCollision(score, laserThree, meteorTen, camera);

                        laserMeteorCollision(score, laserFour, meteorOne, camera);
                        laserMeteorCollision(score, laserFour, meteorTwo, camera);
                        laserMeteorCollision(score, laserFour, meteorThree, camera);
                        laserMeteorCollision(score, laserFour, meteorFour, camera);
                        laserMeteorCollision(score, laserFour, meteorFive, camera);
                        laserMeteorCollision(score, laserFour, meteorSix, camera);
                        laserMeteorCollision(score, laserFour, meteorSeven, camera);
                        laserMeteorCollision(score, laserFour, meteorEight, camera);
                        laserMeteorCollision(score, laserFour, meteorNine, camera);
                        laserMeteorCollision(score, laserFour, meteorTen, camera);

                        laserMeteorCollision(score, laserFive, meteorOne, camera);
                        laserMeteorCollision(score, laserFive, meteorTwo, camera);
                        laserMeteorCollision(score, laserFive, meteorThree, camera);
                        laserMeteorCollision(score, laserFive, meteorFour, camera);
                        laserMeteorCollision(score, laserFive, meteorFive, camera);
                        laserMeteorCollision(score, laserFive, meteorSix, camera);
                        laserMeteorCollision(score, laserFive, meteorSeven, camera);
                        laserMeteorCollision(score, laserFive, meteorEight, camera);
                        laserMeteorCollision(score, laserFive, meteorNine, camera);
                        laserMeteorCollision(score, laserFive, meteorTen, camera);

                        laserMeteorCollision(score, laserSix, meteorOne, camera);
                        laserMeteorCollision(score, laserSix, meteorTwo, camera);
                        laserMeteorCollision(score, laserSix, meteorThree, camera);
                        laserMeteorCollision(score, laserSix, meteorFour, camera);
                        laserMeteorCollision(score, laserSix, meteorFive, camera);
                        laserMeteorCollision(score, laserSix, meteorSix, camera);
                        laserMeteorCollision(score, laserSix, meteorSeven, camera);
                        laserMeteorCollision(score, laserSix, meteorEight, camera);
                        laserMeteorCollision(score, laserSix, meteorNine, camera);
                        laserMeteorCollision(score, laserSix, meteorTen, camera);

                        laserMeteorCollision(score, laserSeven, meteorOne, camera);
                        laserMeteorCollision(score, laserSeven, meteorTwo, camera);
                        laserMeteorCollision(score, laserSeven, meteorThree, camera);
                        laserMeteorCollision(score, laserSeven, meteorFour, camera);
                        laserMeteorCollision(score, laserSeven, meteorFive, camera);
                        laserMeteorCollision(score, laserSeven, meteorSix, camera);
                        laserMeteorCollision(score, laserSeven, meteorSeven, camera);
                        laserMeteorCollision(score, laserSeven, meteorEight, camera);
                        laserMeteorCollision(score, laserSeven, meteorNine, camera);
                        laserMeteorCollision(score, laserSeven, meteorTen, camera);

                        laserMeteorCollision(score, laserEight, meteorOne, camera);
                        laserMeteorCollision(score, laserEight, meteorTwo, camera);
                        laserMeteorCollision(score, laserEight, meteorThree, camera);
                        laserMeteorCollision(score, laserEight, meteorFour, camera);
                        laserMeteorCollision(score, laserEight, meteorFive, camera);
                        laserMeteorCollision(score, laserEight, meteorSix, camera);
                        laserMeteorCollision(score, laserEight, meteorSeven, camera);
                        laserMeteorCollision(score, laserEight, meteorEight, camera);
                        laserMeteorCollision(score, laserEight, meteorNine, camera);
                        laserMeteorCollision(score, laserEight, meteorTen, camera);

                        laserMeteorCollision(score, laserNine, meteorOne, camera);
                        laserMeteorCollision(score, laserNine, meteorTwo, camera);
                        laserMeteorCollision(score, laserNine, meteorThree, camera);
                        laserMeteorCollision(score, laserNine, meteorFour, camera);
                        laserMeteorCollision(score, laserNine, meteorFive, camera);
                        laserMeteorCollision(score, laserNine, meteorSix, camera);
                        laserMeteorCollision(score, laserNine, meteorSeven, camera);
                        laserMeteorCollision(score, laserNine, meteorEight, camera);
                        laserMeteorCollision(score, laserNine, meteorNine, camera);
                        laserMeteorCollision(score, laserNine, meteorTen, camera);

                        laserMeteorCollision(score, laserTen, meteorOne, camera);
                        laserMeteorCollision(score, laserTen, meteorTwo, camera);
                        laserMeteorCollision(score, laserTen, meteorThree, camera);
                        laserMeteorCollision(score, laserTen, meteorFour, camera);
                        laserMeteorCollision(score, laserTen, meteorFive, camera);
                        laserMeteorCollision(score, laserTen, meteorSix, camera);
                        laserMeteorCollision(score, laserTen, meteorSeven, camera);
                        laserMeteorCollision(score, laserTen, meteorEight, camera);
                        laserMeteorCollision(score, laserTen, meteorNine, camera);
                        laserMeteorCollision(score, laserTen, meteorTen, camera);

                        meteorPlayerCollision(alive, spaceshipOne, meteorOne, camera);
                        meteorPlayerCollision(alive, spaceshipOne, meteorTwo, camera);
                        meteorPlayerCollision(alive, spaceshipOne, meteorThree, camera);
                        meteorPlayerCollision(alive, spaceshipOne, meteorFour, camera);
                        meteorPlayerCollision(alive, spaceshipOne, meteorFive, camera);
                        meteorPlayerCollision(alive, spaceshipOne, meteorSix, camera);
                        meteorPlayerCollision(alive, spaceshipOne, meteorSeven, camera);
                        meteorPlayerCollision(alive, spaceshipOne, meteorEight, camera);
                        meteorPlayerCollision(alive, spaceshipOne, meteorNine, camera);
                        meteorPlayerCollision(alive, spaceshipOne, meteorTen, camera);
                        }
                    }
                    //Camera ends
                    camera.EndMode(); 

                    text.Draw("Score: " + std::to_string(score), 10, 10, 20, raylib::Color::RayWhite());
                    break;
                case WIN:
                    //Draws win screen
                    text.Draw(winText, winOnePos.x, winOnePos.y - 50, 40, YELLOW);
                    text.Draw(winTextTwo, winTwoPos.x, winTwoPos.y + 45, 15, RAYWHITE);
                    break;
                case LOSE:
                    //Draws lose screen
                    DrawRectangle(screenWidth/2 - 50, screenHeight/2 + 20, 100, 50, GetColor(0x052c46ff));
                    text.Draw(loseText, losePos.x, losePos.y - 50, 30, RED);
                    text.Draw(restartText, restartPos.x, restartPos.y + 45, 20, RAYWHITE);
                    break;
                default: break;
            }

        }
        EndDrawing();
    }

    //Unload and closes program
    UnloadModel(spaceship);
    UnloadModel(meteor);
    UnloadSound(laserSound);
    CloseAudioDevice();
}