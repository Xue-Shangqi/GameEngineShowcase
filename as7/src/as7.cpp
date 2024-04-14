/*
Author: Shangqi Xue 
Date: March 2rd, 2024
*/ 

#include <BufferedInput.hpp>
#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "EC.hpp"

struct Bowl{
    //Variables for position and speed 
    float speed = 1;
    raylib::Degree tiltTarget = 0;
    float tiltSpeed = 80.0f;
    const float acceleration = 10.0f;
    const float deceleration = 0.1f;
    raylib::Vector3 velocity = {0, 0, 0};      
    raylib::Vector3 pos = {0, 0, 0};    
};

struct RenderComponent : public Component {
    using Component::Component;
    raylib::Model model;
    RenderComponent(Entity& e, raylib::Model&& model): Component(e), model(std::move(model)) { }
    void Tick(float dt) override {
        raylib::Transform ogTransform = model.transform;
        model.transform = raylib::Transform(model.transform).Translate(Transform().position);
        model.transform = raylib::Transform(model.transform).RotateY(Transform().heading);
        model.Draw({});
        model.transform = ogTransform;
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

struct CollisionComponent : public Component {
    raylib::Camera camera;
    Rectangle selfBox;
    CollisionComponent(Entity& e) : Component(e) { }

    void checkCollision(raylib::Camera& camera, Rectangle box, int& score){
        selfBox = {GetWorldToScreen(Transform().position, camera).x - 60,  GetWorldToScreen(Transform().position, camera).y - 365, 120.0f, 60.0f};
        Rectangle bowlBox = box;

        if(CheckCollisionRecs(selfBox, bowlBox)) {
            score += 10;
            BananaController& controller = Object().GetComponent<BananaController>().value();
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
enum CurScreen {START = 0, PLAY, WIN, LOSE};


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
    bowl.AddComponent<RenderComponent>(std::move(bowlModel));
    bowl.AddComponent<BufferedInputComponent>(std::move(input));
    RenderComponent& BowlRenderComp = bowl.GetComponent<RenderComponent>().value();
    BufferedInputComponent& buffer = bowl.GetComponent<BufferedInputComponent>().value();
    TransformComponent& trans = bowl.GetComponent<TransformComponent>().value();
    BowlRenderComp.model.transform = raylib::Transform(BowlRenderComp.model.transform).RotateXYZ(raylib::Degree(90), raylib::Degree(180), raylib::Degree(180)).Translate(0, -40, 0).Scale(5, 5, 5);



    //Init banana 
    std::vector<Entity> bananaList;

    for(int i = 0; i < 5; ++i){
        Entity banana;
        raylib::Model meteorModel = LoadModel("customModel/banana.glb");
        banana.AddComponent<RenderComponent>(std::move(meteorModel));
        banana.AddComponent<BananaController>();   
        RenderComponent& BananaRenderComp = banana.GetComponent<RenderComponent>().value();
        BananaRenderComp.model.transform = raylib::Transform(BananaRenderComp.model.transform).Scale(15, 15, 15).Translate(0, 60, 0);
        banana.AddComponent<CollisionComponent>();    
        bananaList.push_back(std::move(banana));
    }

    // Storage for all collision box


    //Set up sound
    InitAudioDevice();
    SetMasterVolume(0.3f);
    
    

    while (!WindowShouldClose()) {
        switch(curScreen){
            case START:
                    //Checks if the mouse clicks the start button
                    mousePosition = GetMousePosition();
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                        if (CheckCollisionPointRec(mousePosition, startButton) && curScreen == START) {
                            curScreen = PLAY;
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
            //Set up for drawing
            ClearBackground(RAYWHITE);
            DrawTexture(background, 0.0f, 2.0f, WHITE);
            
            
            switch(curScreen){
                case START:
                    //Draws the title of game
                    DrawRectangle(screenWidth/2 - 50, screenHeight/2 + 20, 100, 50, GetColor(0x052c46ff));
                    text.Draw(title, titlePos.x, titlePos.y - 50, 30, BLACK);
                    text.Draw(startText, startPos.x, startPos.y + 45, 20, RAYWHITE);

                    //Reset stage
                    score = 0;
                    alive = true;
                    trans.position = raylib::Vector3{0, 0, 0};
                    trans.heading = raylib::Degree(0);
                    for(int i = 0; i < 5; ++i){
                        BananaController& controller = bananaList[i].GetComponent<BananaController>().value();
                        controller.exist = false;
                        controller.alive = true;
                    }

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
                    //Camera starts
                    camera.BeginMode();
                    {
                        bowlBox = {GetWorldToScreen(trans.position, camera).x - 70,  GetWorldToScreen(trans.position, camera).y + 160, 140.0f, 80.0f};
                        bowl.Tick(GetFrameTime());
                        bananaList[0].Tick(GetFrameTime());
                        for(int i = 0; i < 5; ++i){
                            bananaList[i].Tick(GetFrameTime());
                            CollisionComponent& col = bananaList[i].GetComponent<CollisionComponent>().value();
                            BananaController& controller = bananaList[i].GetComponent<BananaController>().value();
                            col.checkCollision(camera, bowlBox, score);
                            if(!controller.alive){
                                alive = false;
                            }
                        }

                    }
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
    UnloadSound(laserSound);
    CloseAudioDevice();
}