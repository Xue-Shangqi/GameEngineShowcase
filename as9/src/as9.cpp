#include <raylib-cpp.hpp>
#include "ECS.hpp"
#include <time.h>

struct Rendering{
    raylib::Model* model;
};

struct TransformComp{
    raylib::Transform ogTransform = raylib::Transform();
    raylib::Transform newTransform = raylib::Transform();
};

struct BowlData{
    //Variables for position and speed 
    float speed = 1;
    float maxSpeed = 3.0f;
    raylib::Degree tiltTarget = 0;
    raylib::Degree heading = 0;
    float tiltSpeed = 80.0f;
    float acceleration = 10.0f;
    float deceleration = 0.1f;
    raylib::Vector3 velocity = {0, 0, 0};      
    raylib::Vector3 pos = {0, 0, 0};    
};

struct StageData{
    float speed = 20; 
    bool exist = false;
    raylib::Vector3 position = {0, 0, 0};   
    raylib::Degree heading = 0; 
    int stage = 0;
};

struct CollisionData{
    Rectangle selfBox = Rectangle();
    int itemType = 0;
};


// Enum for different screens
enum CurScreen {START = 0, PLAY, WIN, LOSE, STORM};

// All functions
void DrawModel(raylib::Model&, TransformComp&);
void DrawSystem(Scene<ComponentStorage>&, CurScreen);
void SpawnSystem(Scene<ComponentStorage>&, bool&, CurScreen, bool);
void SetupSystem(Scene<ComponentStorage>&, raylib::Model&, int);
void SetupPlayer(Scene<ComponentStorage>&, raylib::Model&);
void InputManager(Scene<ComponentStorage>&);
void CollisionSystem(Scene<ComponentStorage>&, raylib::Camera, CurScreen, int&, bool, raylib::Sound&);
Vector2 calcTextMid(std::string, int);

void DrawModel(raylib::Model& model, TransformComp& transformcomp){
	raylib::Transform backupTransform = model.transform;
	model.transform = transformcomp.newTransform;
	model.Draw({});
	model.transform = backupTransform;
	transformcomp.newTransform = transformcomp.ogTransform;
}

void DrawSystem(Scene<ComponentStorage>& scene, CurScreen curscreen){
    for(Entity e = 0; e < scene.entityMasks.size(); e++){
        if(!scene.HasComponent<Rendering>(e)) continue;
        if(!scene.HasComponent<TransformComp>(e)) continue;
        auto& rendering = scene.GetComponent<Rendering>(e);
        auto& transformComp = scene.GetComponent<TransformComp>(e);


        if(scene.HasComponent<BowlData>(e)){
            DrawModel(*rendering.model, transformComp);
        }

        if(scene.HasComponent<StageData>(e)){
            auto& stagedata = scene.GetComponent<StageData>(e);
            if(stagedata.stage == 1 && curscreen == PLAY)
                DrawModel(*rendering.model, transformComp);
            if(stagedata.stage == 2 && curscreen == STORM)
                DrawModel(*rendering.model, transformComp);
        }
    }

}

void SetupSystem(Scene<ComponentStorage>& scene, raylib::Model& model, int stage){
	Entity e = scene.CreateEntity();
	auto& rendering = scene.AddComponent<Rendering>(e);
	rendering.model = &model;
    auto& stageInfo = scene.AddComponent<StageData>(e);
    stageInfo.stage = stage;
	auto& transform = scene.AddComponent<TransformComp>(e);
	transform.ogTransform = rendering.model->transform;
	transform.newTransform = transform.ogTransform;
}

void SetupPlayer(Scene<ComponentStorage>& scene, raylib::Model& model){
	Entity e = scene.CreateEntity();
	auto& rendering = scene.AddComponent<Rendering>(e);
	rendering.model = &model;
    scene.AddComponent<BowlData>(e);
    scene.AddComponent<CollisionData>(e);
	auto& transform = scene.AddComponent<TransformComp>(e);
	transform.ogTransform = rendering.model->transform;
	transform.newTransform = transform.ogTransform;
}

void SpawnSystem(Scene<ComponentStorage>& scene, bool& alive, CurScreen screen, bool rightTime){
    float speed = GetRandomValue(15, 20); 
    float spinRate = GetRandomValue(50, 100);
    if(screen == PLAY){
        for(Entity e = 1; e <= 10; ++e){
            auto& transformcomp = scene.GetComponent<TransformComp>(e);
            auto& data = scene.GetComponent<StageData>(e);
            if(!data.exist){
                int randomX = GetRandomValue(-70, 70);
                int randomY = GetRandomValue(0, 200);
                data.position.x = randomX;
                data.position.y = randomY;
                data.exist = true;
            }

            if(data.exist && data.position.y > -120){
                data.position.y -= speed * GetFrameTime();
                data.heading += spinRate * GetFrameTime();
            }else if(data.position.y < -120){
                data.position.y = 0;
                data.exist = false;
                alive = false;
            }
            transformcomp.newTransform = transformcomp.ogTransform.Translate(data.position).RotateY(data.heading);
        }        
    }else if(screen == STORM){
        int amount = 16;
        if(rightTime) 
            amount = 30;
        for(Entity e = 11; e <= amount; ++e){
            auto& transformcomp = scene.GetComponent<TransformComp>(e);
            auto& data = scene.GetComponent<StageData>(e);
            if(!data.exist){
                int randomX = GetRandomValue(-70, 70);
                int randomY = GetRandomValue(0, 200);
                data.position.x = randomX;
                data.position.y = randomY;
                data.exist = true;
            }

            if(data.exist && data.position.y > -120){
                if(rightTime){ speed = GetRandomValue(70, 100);}
                data.position.y -= speed * GetFrameTime();
                data.heading += spinRate * GetFrameTime();
            }else if(data.position.y < -120){
                data.position.y = 0;
                data.exist = false;
                alive = false;
            }
            transformcomp.newTransform = transformcomp.ogTransform.Translate(data.position).RotateY(data.heading);
        }  
    }

}

void InputManager(Scene<ComponentStorage>& scene){
    auto& data = scene.GetComponent<BowlData>(0);
    auto& transform = scene.GetComponent<TransformComp>(0);
    float dt = GetFrameTime();
    if(IsKeyDown(KEY_A)){
        data.velocity.x += data.acceleration * dt;
        data.tiltTarget = -30;
    }
    if(IsKeyDown(KEY_D)){
        data.velocity.x -= data.acceleration * dt;
        data.tiltTarget = 30;
    }
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

    data.velocity.x = (data.velocity.x > data.maxSpeed) ? data.maxSpeed : ((data.velocity.x < -data.maxSpeed) ? -data.maxSpeed : data.velocity.x);
    data.pos += data.velocity;

    if(data.heading < data.tiltTarget){
        data.heading += data.tiltSpeed * dt;
    }
    if(data.heading > data.tiltTarget){
        data.heading -= data.tiltSpeed * dt;
    }

    if (data.pos.x > 80) {
        data.pos.x = -80;
    } else if (data.pos.x < -80) {
        data.pos.x = 80;
    } 

    transform.newTransform = transform.ogTransform.Translate(data.pos).RotateY(data.heading);
}

void CollisionSystem(Scene<ComponentStorage>& scene, raylib::Camera camera, CurScreen screen, int& score, bool mute, raylib::Sound& collect){
    auto& player = scene.GetComponent<CollisionData>(0);
    auto& playerTransform = scene.GetComponent<BowlData>(0);
    player.selfBox = {GetWorldToScreen(playerTransform.pos, camera).x - 70,  GetWorldToScreen(playerTransform.pos, camera).y + 160, 140.0f, 80.0f};
    
    if(screen == PLAY){
        for(Entity e = 1; e <= 10; ++e){
            auto& other = scene.GetComponent<CollisionData>(e);
            auto& otherData = scene.GetComponent<StageData>(e);
            switch(other.itemType){
                case 1:
                    other.selfBox = {GetWorldToScreen(otherData.position, camera).x - 60,  GetWorldToScreen(otherData.position, camera).y - 365, 120.0f, 60.0f};
                    break;
                case 2:
                    other.selfBox = {GetWorldToScreen(otherData.position, camera).x - 25,  GetWorldToScreen(otherData.position, camera).y - 380, 50.0f, 60.0f};
                    break;
                case 3:
                    other.selfBox = {GetWorldToScreen(otherData.position, camera).x - 32,  GetWorldToScreen(otherData.position, camera).y - 355, 65.0f, 60.0f};;
                    break;
                case 4:
                    other.selfBox = {GetWorldToScreen(otherData.position, camera).x - 40,  GetWorldToScreen(otherData.position, camera).y - 455, 80.0f, 130.0f};
                    break;
                default: break;
            }
            
            if(CheckCollisionRecs(player.selfBox, other.selfBox)){
                if(!mute){
                    collect.Play();
                }
                score += 10;
                otherData.exist = false;
            }
        }
    }else if(screen == STORM){
        for(Entity e = 11; e < scene.entityMasks.size(); ++e){
            auto& other = scene.GetComponent<CollisionData>(e);
            auto& otherData = scene.GetComponent<StageData>(e);
            other.selfBox = {GetWorldToScreen(otherData.position, camera).x - 20,  GetWorldToScreen(otherData.position, camera).y - 445, 140.0f, 120.0f};
            
            if(CheckCollisionRecs(player.selfBox, other.selfBox)){
                if(!mute){
                    collect.Play();
                }
                score += 10;
                otherData.exist = false;
            }
        }
    }
}

Vector2 calcTextMid(std::string content, int font){
    int textWidth = MeasureText(content.c_str(), font);
    int textHeight = font;
    float posX = (800 - textWidth) / 2;
    float posY = (450 - textHeight) / 2;

    return Vector2{posX, posY};
}

int main(){
    // Initialize window 
    int screenWidth = 800;
    int screenHeight = 450;
    SetTargetFPS(60);
    InitWindow(screenWidth, screenHeight, "CS381 - Assignment 9");
    CurScreen curScreen = START;
    int score = 0;
    bool alive = true;

    // Set up camera
    raylib::Camera camera(
        raylib::Vector3(0, 0, -100),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );
    
    // Initialize all text in the game
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

    // Initialize mouse and buttons
    Vector2 mousePosition = GetMousePosition();
    Rectangle startButton = {(float)(screenWidth / 2 - 50), (float)(screenHeight / 2 + 20), 100, 50};
    Rectangle funnyButton = {(float)(screenWidth / 2 - 130), (float)(screenHeight / 2 + 80), 260, 50};
    Rectangle muteButton = {(float)(screenWidth / 2 - 390), (float)(screenHeight / 2 + 170), 40, 20};

    // Load background
    Texture2D background = LoadTexture("textures/background.png");

    // Load Models
    raylib::Model bowl = LoadModel("customModel/wooden_bowl.glb");
    bowl.transform = raylib::Transform(bowl.transform).RotateXYZ(raylib::Degree(90), raylib::Degree(180), raylib::Degree(180)).Translate(0, -40, 0).Scale(5, 5, 5);
    raylib::Model banana = LoadModel("customModel/banana.glb");
    banana.transform = raylib::Transform(banana.transform).Scale(15, 15, 15).Translate(0, 60, 0);
    raylib::Model strawberry = LoadModel("customModel/strawberry.glb");
    strawberry.transform = raylib::Transform(strawberry.transform).Scale(5, 5, 5).Translate(0, 60, 0).RotateXYZ(raylib::Degree(90), raylib::Degree(180), raylib::Degree(0));
    raylib::Model apple = LoadModel("customModel/apple.glb");
    apple.transform = raylib::Transform(apple.transform).Scale(.12, .12, .12).Translate(0, 60, 0).RotateXYZ(raylib::Degree(-90), raylib::Degree(0), raylib::Degree(0));
    raylib::Model pineapple = LoadModel("customModel/pineapple.glb");
    pineapple.transform = raylib::Transform(pineapple.transform).Scale(7, 7, 7).Translate(0, 60, 0).RotateXYZ(raylib::Degree(-90), raylib::Degree(0), raylib::Degree(0));
    raylib::Model sword = LoadModel("customModel/vergils_yamato_v2.glb");
    sword.transform = raylib::Transform(sword.transform).Scale(.5, .5, .5).Translate(0, 60, 0).RotateXYZ(raylib::Degree(-45), raylib::Degree(0), raylib::Degree(0));;

    // Initialize Scene 
    Scene scene = Scene();
    SetupPlayer(scene, bowl);
    auto& playerData = scene.GetComponent<BowlData>(0);
    for(Entity e = 1; e <= 10; ++e){
        int random = GetRandomValue(1, 4); 
        if(random == 1){
            SetupSystem(scene, banana, 1);
            auto& collisionData = scene.AddComponent<CollisionData>(e);
            collisionData.itemType = 1;              
        }
        if(random == 2){
            SetupSystem(scene, strawberry, 1);
            auto& collisionData = scene.AddComponent<CollisionData>(e);
            collisionData.itemType = 2;            
        }
        if(random == 3){
            SetupSystem(scene, apple, 1);
            auto& collisionData = scene.AddComponent<CollisionData>(e);
            collisionData.itemType = 3;           
        }
        if(random == 4){
            SetupSystem(scene, pineapple, 1);
            auto& collisionData = scene.AddComponent<CollisionData>(e);
            collisionData.itemType = 4;            
        }
    }
    
    for(Entity e = 11; e <= 30; ++e){
        SetupSystem(scene, sword, 2);
        auto& collisionData = scene.AddComponent<CollisionData>(e);
        collisionData.itemType = 10;
    }

    // Time tracker
    time_t start, end, tracker;
    double elapsed_seconds, tracker_seconds;
    bool startTimeCaptured = false;
    bool timeCaptured = false;
    bool rightTime = false;

    // Initialize audio 
    InitAudioDevice();
    raylib::Sound collect ("sound/collected_item.mp3");
    // Credit to pear8737 and Nj0820 for background music
    raylib::Music backgroundMusic ("sound/music-background.mp3");
    raylib::Music stormMusic ("sound/storm.mp3");
    SetMasterVolume(0.3f);
    bool mute = false;

    while(!WindowShouldClose()){
        mousePosition = GetMousePosition();

        // Sound controls 
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if (CheckCollisionPointRec(mousePosition, muteButton)) {
                mute = !mute;
            }
        }

        if(!mute && curScreen != STORM){
            if(stormMusic.IsPlaying()){
                stormMusic.Stop();
            }
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
                // Checks if the mouse clicks the start button
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
                if(score >= 200){
                    curScreen = WIN;
                }

                if(!alive){
                    curScreen = LOSE;
                }
                break;
            case STORM:
                if(score >= 950){
                    curScreen = WIN;
                }

                if(!alive){
                    curScreen = LOSE;
                }
                break;
            case WIN:
                break;
            case LOSE:
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
            // Set up for drawing
            ClearBackground(RAYWHITE);
            DrawTexture(background, 0.0f, 2.0f, WHITE);
            DrawRectangleRec(muteButton, GRAY);
            text.Draw("MUTE", (float)(screenWidth / 2 - 385), (float)(screenHeight / 2 + 175), 10, raylib::Color::RayWhite());

            // Used for testing 
            // auto& tempdata = scene.GetComponent<StageData>(1);
            // collisionData.selfBox = {GetWorldToScreen(tempdata.position, camera).x - 20,  GetWorldToScreen(tempdata.position, camera).y - 445, 140.0f, 120.0f};
            // DrawRectangleRec(collisionData.selfBox, RED);

            switch(curScreen){
                case START:
                    // Draw start game informations
                    DrawRectangle(screenWidth/2 - 50, screenHeight/2 + 20, 100, 50, GetColor(0x052c46ff));
                    DrawRectangleRec(funnyButton, GetColor(0x7DF9FF));
                    text.Draw(title, titlePos.x, titlePos.y - 50, 30, BLACK);
                    text.Draw(startText, startPos.x, startPos.y + 45, 20, RAYWHITE);
                    text.Draw(funnyMode, funnyModePos.x, funnyModePos.y + 100, 20, RAYWHITE);
                    text.Draw(warning, funnyModePos.x + 50, funnyModePos.y + 125, 15, RAYWHITE);

                    // Reset all variables
                    
                    startTimeCaptured, timeCaptured = false;
                    score = 0;
                    alive = true;
                    timeCaptured = false;
                    startTimeCaptured = false;
                    rightTime = false;
                    playerData.pos = raylib::Vector3{0, 0, 0};
                    playerData.heading = raylib::Degree(0);
                    playerData.maxSpeed = 2.5f;
                    playerData.acceleration = 10.0f;
                    playerData.velocity = raylib::Vector3{0, 0, 0};
                    playerData.maxSpeed = 3.0f;
                    playerData.acceleration = 10.0f;
                    tracker_seconds = 0;

                    for(Entity e = 1; e < scene.entityMasks.size(); ++e){
                        auto& fruitData = scene.GetComponent<StageData>(e);
                        fruitData.position.y = 200;
                        fruitData.exist = false;
                    }

                    break;
                case PLAY:
                    if(!startTimeCaptured){
                        time(&start);
                        startTimeCaptured = true;
                    }

                    camera.BeginMode();
                    {
                        DrawSystem(scene, curScreen);
                        SpawnSystem(scene, alive, curScreen, rightTime);
                        InputManager(scene);
                        CollisionSystem(scene, camera, curScreen, score, mute, collect);

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
                        playerData.maxSpeed = 10.0f;
                        playerData.acceleration = 100.0f;
                    }

                    camera.BeginMode();
                    {
                        DrawSystem(scene, curScreen);
                        SpawnSystem(scene, alive, curScreen, rightTime);
                        InputManager(scene);
                        CollisionSystem(scene, camera, curScreen, score, mute, collect);

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
                    // Draws win screen
                    text.Draw(winText, winOnePos.x, winOnePos.y - 50, 40, YELLOW);
                    text.Draw(winTextTwo + std::to_string((int)elapsed_seconds) + "s", winTwoPos.x, winTwoPos.y + 45, 15, RAYWHITE);
                    break;
                case LOSE:
                    // Draws lose screen
                    DrawRectangle(screenWidth/2 - 50, screenHeight/2 + 20, 100, 50, GetColor(0x052c46ff));
                    text.Draw(loseText, losePos.x, losePos.y - 50, 30, RED);
                    text.Draw(restartText, restartPos.x, restartPos.y + 45, 20, RAYWHITE);
                    break;
                default: break;
            }

        }
        EndDrawing();
    }
    
    CloseAudioDevice();
    return 0;
}