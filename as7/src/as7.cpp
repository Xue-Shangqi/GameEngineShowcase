/*
Author: Shangqi Xue 
Date: April 14th, 2024
*/ 

#include <BufferedInput.hpp>
#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "EC.hpp"
#include <chrono>
#include <time.h>

struct Bowl{
    //Variables for position and speed 
    float speed = 1;
    raylib::Degree tiltTarget = 0;
    float tiltSpeed = 80.0f;
    float acceleration = 10.0f;
    float deceleration = 0.1f;
    raylib::Vector3 velocity = {0, 0, 0};      
    raylib::Vector3 pos = {0, 0, 0};    
};

struct RenderComponent : public Component {
    using Component::Component;
    raylib::Model* model;
    RenderComponent(Entity& e, raylib::Model* model): Component(e), model(model) { }
    void Tick(float dt) override {
        raylib::Transform ogTransform = model->transform;
        model->transform = raylib::Transform(model->transform).Translate(Transform().position);
        model->transform = raylib::Transform(model->transform).RotateY(Transform().heading);
        model->Draw({});
        model->transform = ogTransform;
    }
};

struct BufferedInputComponent : public Component {
    using Component::Component;
    raylib::BufferedInput input;
    Bowl data;
    int maxSpeed = 2.5f;
    BufferedInputComponent(Entity& e, raylib::BufferedInput&& input): Component(e), input(std::move(input)) { }

    void Tick(float dt) override {
        input.PollEvents();

        // Buffer input for movement
        input["a"] = raylib::Action::key(KEY_A).SetPressedCallback([&]{
            data.velocity.x += data.acceleration * dt;
            data.tiltTarget = -30;
		}).move();

        input["d"] = raylib::Action::key(KEY_D).SetPressedCallback([&]{
            data.velocity.x -= data.acceleration * dt;
            data.tiltTarget = 30;
		}).move();

        // Set velocity to 0 when key is released
        if(IsKeyReleased(KEY_A)){
            data.tiltTarget = 0;
            while(data.velocity.x > 0){
                data.velocity.x -= data.deceleration * dt;
            }
        }
        if(IsKeyReleased(KEY_D)){
            data.tiltTarget = 0;
            while(data.velocity.x < 0){
                data.velocity.x += data.deceleration * dt;
            }
        }

        // Limit speed to max speed
        data.velocity.x = (data.velocity.x > maxSpeed) ? maxSpeed : ((data.velocity.x < -maxSpeed) ? -maxSpeed : data.velocity.x);

        // Updates the position of ship
        Transform().position.x += data.velocity.x;
        Transform().position.y += data.velocity.y;

        // Tilt the ship when moving side 
        if(Transform().heading < data.tiltTarget){
            Transform().heading += data.tiltSpeed * dt;
        }
        if(Transform().heading > data.tiltTarget){
            Transform().heading -= data.tiltSpeed * dt;
        }
        //Warps the player around screen
        if (Transform().position.x > 80) {
            Transform().position.x = -80;
        } else if (Transform().position.x < -80) {
            Transform().position.x = 80;
        } 
    } 
};

struct BananaController : public Component {
    float speed = 20; 
    bool exist = false;
    bool alive = true;
    BananaController(Entity& e) : Component(e) { }
    
    void Tick(float dt) override {
        if(!exist){
            int randomX = GetRandomValue(-70, 70);
            int randomY = GetRandomValue(0, 200);
            Transform().position.x = randomX;
            Transform().position.y = randomY;
            exist = true;
        }

        if(exist && Transform().position.y > -120){
            Transform().position.y -= speed * dt;
        }else if(Transform().position.y < -120){
            Transform().position.y = 0;
            exist = false;
            alive = false;
        }
    }
};

struct SwordController : public Component {
    float speed = GetRandomValue(20, 40); 
    bool exist = false;
    bool alive = true;
    SwordController(Entity& e) : Component(e) { }
    
    void Tick(float dt) override {
        if(!exist){
            int randomX = GetRandomValue(-70, 70);
            int randomY = GetRandomValue(0, 200);
            Transform().position.x = randomX;
            Transform().position.y = randomY;
            exist = true;
        }

        if(exist && Transform().position.y > -120){
            Transform().position.y -= speed * dt;
        }else if(Transform().position.y < -120){
            Transform().position.y = 0;
            exist = false;
            alive = false;
        }
    }

    void changeSpeed(int newSpeed){
        speed = newSpeed;
    }
};

struct CollisionComponentBanana : public Component {
    raylib::Camera camera;
    Rectangle selfBox;
    CollisionComponentBanana(Entity& e) : Component(e) { }

    void checkCollision(raylib::Camera& camera, Rectangle box, int& score, raylib::Sound& collect, bool mute){
        selfBox = {GetWorldToScreen(Transform().position, camera).x - 60,  GetWorldToScreen(Transform().position, camera).y - 365, 120.0f, 60.0f};
        Rectangle bowlBox = box;

        if(CheckCollisionRecs(selfBox, bowlBox)) {
            score += 10;
            if(!mute){
                collect.Play();
            }
            BananaController& controller = Object().GetComponent<BananaController>().value();
            controller.exist = false;
        }
    }
};

struct CollisionComponentSword : public Component {
    raylib::Camera camera;
    Rectangle selfBox;
    CollisionComponentSword(Entity& e) : Component(e) { }

    void checkCollision(raylib::Camera& camera, Rectangle box, int& score, raylib::Sound& collect, bool mute){
        selfBox = {GetWorldToScreen(Transform().position, camera).x - 160,  GetWorldToScreen(Transform().position, camera).y - 375, 200.0f, 60.0f};
        Rectangle bowlBox = box;

        if(CheckCollisionRecs(selfBox, bowlBox)) {
            score += 10;
            if(!mute){
                collect.Play();
            }
            SwordController& controller = Object().GetComponent<SwordController>().value();
            controller.exist = false;
        }
    }
};

//Calculate center text 
Vector2 calcTextMid(std::string content, int font){
    int textWidth = MeasureText(content.c_str(), font);
    int textHeight = font;
    float posX = (800 - textWidth) / 2;
    float posY = (450 - textHeight) / 2;

    return Vector2{posX, posY};
}

//Enum for different screens
enum CurScreen {START = 0, PLAY, WIN, LOSE, STORM};

int main(){
    //Initialize window 
    int screenWidth = 800;
    int screenHeight = 450;
    SetTargetFPS(60);
    InitWindow(screenWidth, screenHeight, "CS381 - Assignment 7");
    int score = 0;
    CurScreen curScreen = START;
    raylib::BufferedInput input;
    bool alive = true;

    //Init for text
    raylib::Text text;
    std::string title = "Very Scuffed Fruit Collecting Game";
    std::string startText = "Start";
    std::string winText = "You Win!!!";
    std::string winTextTwo = "you've officially wasted: ";
    std::string loseText = "WOMP WOMP, L + Ratio";
    std::string restartText = "Restart?";
    std::string funnyMode = "Funny Challenge Mode?";
    std::string warning = "(HAVE SOUND ON!!)";
    Vector2 titlePos = calcTextMid(title, 30);
    Vector2 startPos = calcTextMid(startText, 20);
    Vector2 winOnePos = calcTextMid(winText, 40);
    Vector2 winTwoPos = calcTextMid(winTextTwo, 15);
    Vector2 losePos = calcTextMid(loseText, 30);
    Vector2 restartPos = calcTextMid(restartText, 20);
    Vector2 funnyModePos = calcTextMid(funnyMode, 20);
    Vector2 warningPos = calcTextMid(warning, 15);

    //Init start button and mouse
    Vector2 mousePosition = GetMousePosition();
    Rectangle startButton = {(float)(screenWidth / 2 - 50), (float)(screenHeight / 2 + 20), 100, 50};
    Rectangle funnyButton = {(float)(screenWidth / 2 - 130), (float)(screenHeight / 2 + 80), 260, 50};
    Rectangle muteButton = {(float)(screenWidth / 2 - 390), (float)(screenHeight / 2 + 170), 40, 20};

    //Load background
    Texture2D background = LoadTexture("textures/background.png");
    float scrollingBack = 0.0f;

    //Set up camera
    raylib::Camera camera(
        raylib::Vector3(0, 0, -100),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );

    //Init bowl
    Entity bowl;
    Rectangle bowlBox;
    raylib::Model bowlModel = LoadModel("customModel/wooden_bowl.glb");
    bowl.AddComponent<RenderComponent>(&bowlModel);
    bowl.AddComponent<BufferedInputComponent>(std::move(input));
    RenderComponent& BowlRenderComp = bowl.GetComponent<RenderComponent>().value();
    BufferedInputComponent& buffer = bowl.GetComponent<BufferedInputComponent>().value();
    TransformComponent& trans = bowl.GetComponent<TransformComponent>().value();
    BowlRenderComp.model->transform = raylib::Transform(BowlRenderComp.model->transform).RotateXYZ(raylib::Degree(90), raylib::Degree(180), raylib::Degree(180)).Translate(0, -40, 0).Scale(5, 5, 5);

    //Init banana 
    std::vector<Entity> bananaList;
    raylib::Model bananaModel = LoadModel("customModel/banana.glb");
    bananaModel.transform = raylib::Transform(bananaModel.transform).Scale(15, 15, 15).Translate(0, 60, 0);
    for(int i = 0; i < 5; ++i){
        Entity banana;
        banana.AddComponent<RenderComponent>(&bananaModel);
        banana.AddComponent<BananaController>();   
        banana.AddComponent<CollisionComponentBanana>();    
        bananaList.push_back(std::move(banana));
    }

    // Init Vergil Sword
    std::vector<Entity> swordList;
    raylib::Model swordModel = LoadModel("customModel/devil_sword_vergil.glb");
    swordModel.transform = raylib::Transform(swordModel.transform).Scale(5, 5, 5).Translate(0, 60, 0);
    for(int i = 0; i < 20; ++i){
        Entity sword;
        sword.AddComponent<RenderComponent>(&swordModel);
        sword.AddComponent<SwordController>();   
        sword.AddComponent<CollisionComponentSword>();    
        swordList.push_back(std::move(sword));
    }
    bool rightTime = false;

    // Time tracker
    time_t start, end, tracker;
    double elapsed_seconds, tracker_seconds;
    bool startTimeCaptured, timeCaptured = false;


    //Set up sound
    InitAudioDevice();
    raylib::Sound collect ("sound/collected_item.mp3");
    // Credit to pear8737 and Nj0820 for background music
    raylib::Music backgroundMusic ("sound/music-background.mp3");
    raylib::Music stormMusic ("sound/storm.mp3");
    SetMasterVolume(0.3f);
    bool mute = false;
    

    while (!WindowShouldClose()) {
        mousePosition = GetMousePosition();
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if (CheckCollisionPointRec(mousePosition, muteButton)) {
                mute = !mute;
            }
        }

        if(!mute && curScreen != STORM){
            backgroundMusic.Play();
            backgroundMusic.Update();            
        }else if(!mute){
            if(backgroundMusic.IsPlaying()){
                backgroundMusic.Stop();
            }
            SetMasterVolume(1.0f);
            stormMusic.Play();
            stormMusic.Update();  
        }else{
            stormMusic.Stop();
            backgroundMusic.Stop();
        }

        switch(curScreen){
            case START:
                //Checks if the mouse clicks the start button
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                    if (CheckCollisionPointRec(mousePosition, startButton) && curScreen == START) {
                        curScreen = PLAY;
                    }
                }
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                    if (CheckCollisionPointRec(mousePosition, funnyButton) && curScreen == START) {
                        curScreen = STORM;
                    }
                }
                break;
            case PLAY:
                if(score >= 100){
                    curScreen = WIN;
                }

                if(!alive){
                    curScreen = LOSE;
                }
                break;
            case STORM:
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
        
        Rectangle selfBox;
        BeginDrawing();
        {   
            //Set up for drawing
            ClearBackground(RAYWHITE);
            DrawTexture(background, 0.0f, 2.0f, WHITE);
            DrawRectangleRec(muteButton, GRAY);
            text.Draw("MUTE", (float)(screenWidth / 2 - 385), (float)(screenHeight / 2 + 175), 10, raylib::Color::RayWhite());
            
            switch(curScreen){
                case START:
                    //Draws the title of game
                    DrawRectangle(screenWidth/2 - 50, screenHeight/2 + 20, 100, 50, GetColor(0x052c46ff));
                    DrawRectangleRec(funnyButton, GetColor(0x7DF9FF));
                    text.Draw(title, titlePos.x, titlePos.y - 50, 30, BLACK);
                    text.Draw(startText, startPos.x, startPos.y + 45, 20, RAYWHITE);
                    text.Draw(funnyMode, funnyModePos.x, funnyModePos.y + 100, 20, RAYWHITE);
                    text.Draw(warning, funnyModePos.x + 50, funnyModePos.y + 125, 15, RAYWHITE);

                    //Reset stage
                    score = 0;
                    alive = true;
                    timeCaptured = false;
                    startTimeCaptured = false;
                    rightTime = false;
                    trans.position = raylib::Vector3{0, 0, 0};
                    trans.heading = raylib::Degree(0);
                    buffer.maxSpeed = 2.5f;
                    buffer.data.acceleration = 10.0f;
                    tracker_seconds = 0;
                    for(int i = 0; i < 5; ++i){
                        BananaController& controller = bananaList[i].GetComponent<BananaController>().value();
                        controller.exist = false;
                        controller.alive = true;
                    }
                    for(int i = 0; i < 20; ++i){
                        SwordController& controller = swordList[i].GetComponent<SwordController>().value();
                        controller.exist = false;
                        controller.alive = true;
                        controller.speed = GetRandomValue(20,40);
                    }
                    startTimeCaptured, timeCaptured = false;

                    //Camera starts
                    camera.BeginMode();
                    {
                        //Draws the model
                        bowl.Tick(GetFrameTime());
                        
                    }
                    //Camera ends
                    camera.EndMode(); 

                    break;
                case PLAY:
                    if(!startTimeCaptured){
                        // start = std::chrono::steady_clock::now();
                        time(&start);
                        startTimeCaptured = true;
                    }

                    //Camera starts
                    camera.BeginMode();
                    {
                        bowlBox = {GetWorldToScreen(trans.position, camera).x - 70,  GetWorldToScreen(trans.position, camera).y + 160, 140.0f, 80.0f};
                        bowl.Tick(GetFrameTime());
                        for(int i = 0; i < 5; ++i){
                            bananaList[i].Tick(GetFrameTime());
                            CollisionComponentBanana& col = bananaList[i].GetComponent<CollisionComponentBanana>().value();
                            BananaController& controller = bananaList[i].GetComponent<BananaController>().value();
                            col.checkCollision(camera, bowlBox, score, collect, mute);
                            if(!controller.alive){
                                alive = false;
                            }
                        }

                    }
                    camera.EndMode(); 

                    text.Draw("Score: " + std::to_string(score), 10, 10, 20, raylib::Color::RayWhite());
                    break;
                case STORM:
                    if(!startTimeCaptured){
                        time(&start);
                        startTimeCaptured = true;
                    }

                    time(&tracker);
                    tracker_seconds = difftime(tracker, start);
                    if(tracker_seconds >= 13){
                        rightTime = true;
                    }

                    camera.BeginMode();
                    {
                        bowlBox = {GetWorldToScreen(trans.position, camera).x - 70,  GetWorldToScreen(trans.position, camera).y + 160, 140.0f, 80.0f};
                        bowl.Tick(GetFrameTime());
                        if(!rightTime){
                            for(int i = 0; i < 3; ++i){
                                swordList[i].Tick(GetFrameTime());
                                CollisionComponentSword& col = swordList[i].GetComponent<CollisionComponentSword>().value();
                                SwordController& controller = swordList[i].GetComponent<SwordController>().value();
                                col.checkCollision(camera, bowlBox, score, collect, mute);
                                if(!controller.alive){
                                    alive = false;
                                }
                            }                            
                        }else{
                            buffer.maxSpeed = 10.0f;
                            buffer.data.acceleration = 100.0f;
                            for(int i = 0; i < 20; ++i){
                                swordList[i].Tick(GetFrameTime());
                                CollisionComponentSword& col = swordList[i].GetComponent<CollisionComponentSword>().value();
                                SwordController& controller = swordList[i].GetComponent<SwordController>().value();
                                controller.changeSpeed(GetRandomValue(70, 100));
                                col.checkCollision(camera, bowlBox, score, collect, mute);
                                if(!controller.alive){
                                    alive = false;
                                }
                            }    
                        }
                    }
                    camera.EndMode(); 

                    text.Draw("Score: " + std::to_string(score), 10, 10, 20, raylib::Color::RayWhite());
                    break;
                case WIN:
                    if(!timeCaptured){
                        time(&end);
                        elapsed_seconds = difftime(end, start);
                        timeCaptured = true;                    
                    }
                    //Draws win screen
                    text.Draw(winText, winOnePos.x, winOnePos.y - 50, 40, YELLOW);
                    text.Draw(winTextTwo + std::to_string((int)elapsed_seconds) + "s", winTwoPos.x, winTwoPos.y + 45, 15, RAYWHITE);
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
    CloseAudioDevice();
}