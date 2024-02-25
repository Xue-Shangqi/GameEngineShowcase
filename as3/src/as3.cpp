    #include "raylib-cpp.hpp"
    #include "rlgl.h"

    //lambda function for translation
    auto translateModel = [](raylib::Model& model, float x, float y, float z){
        model.transform = raylib::Transform(model.transform).Translate(x,y,z);
    };

    //lambda function for clamp
    static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
        int intPart = angle;
        float floatPart = float(angle) - intPart;
        intPart %= 360;
        intPart += (intPart < 0) * 360;
        return intPart + floatPart;
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

    void movementControl(Plane plane){
        //Movement conditions
        //Backward
        if(IsKeyDown(KEY_W)  && !plane.isBackDown){
            plane.speedTarget += 10;
        }
        plane.isBackDown = (IsKeyDown(KEY_W));
        
        //Forward
        if(IsKeyDown(KEY_S) && !plane.isFrontDown){
            plane.speedTarget -= 10;
        }
        plane.isFrontDown = (IsKeyDown(KEY_S));

        //Stops the plane
        if(IsKeyDown(KEY_SPACE) && !plane.isSpaceDown){
            plane.speedTarget = 0;
        }
        plane.isSpaceDown = (IsKeyDown(KEY_SPACE));

        //Right 
        if(IsKeyDown(KEY_A) && !plane.isRightDown){
            plane.headingTarget += 60;
        }
        plane.isRightDown = (IsKeyDown(KEY_A));

        //Left
        if(IsKeyDown(KEY_D) && !plane.isLeftDown){
            plane.headingTarget -= 60;
        }
        plane.isLeftDown = (IsKeyDown(KEY_D));

        //Ascend
        // if(IsKeyDown(KEY_Q) && !isQDown){
        //     velocity += raylib::Vector3::Up() * speed;
        // }
        // isQDown = IsKeyDown(KEY_Q);

        // //Descend
        // if(IsKeyDown(KEY_E) && !isEDown){
        //     velocity += raylib::Vector3::Down() * speed;
        // }
        // isEDown = IsKeyDown(KEY_E);

        //Speed calculation
        if(plane.speedTarget > plane.speed){
            plane.speed += plane.acceleration * GetFrameTime();
        }else if(plane.speedTarget < plane.speed){
            plane.speed -= plane.acceleration * GetFrameTime();
        }
        plane.velocity = raylib::Vector3{-plane.speed * cos(plane.heading.RadianValue()), 0, plane.speed * sin(plane.heading.RadianValue())};
        plane.pos += plane.velocity * GetFrameTime();
        

        //Heading calculation
        plane.headingTarget = AngleClamp(plane.headingTarget);
        plane.heading = AngleClamp(plane.heading);
        float difference = abs(plane.headingTarget - plane.heading);
        if(plane.headingTarget > plane.heading){
            if(difference < 180) plane.heading += plane.acceleration * GetFrameTime();
            else if(difference > 180) plane.heading -= plane.acceleration * GetFrameTime();
        }else if(plane.headingTarget < plane.heading){
            if(difference < 180) plane.heading -= plane.acceleration * GetFrameTime();
            else if(difference > 180) plane.heading += plane.acceleration * GetFrameTime();
        }
    }

    struct Skybox {
        raylib::Texture texture;
        raylib::Shader shader;
        raylib::Model cube;

        Skybox(): shader(0) {}

        void Init(){
            auto gen = raylib::Mesh::Cube(1.0f, 1.0f, 1.0f);
            cube = ((raylib::Mesh*)(&gen))->LoadModelFrom();

            shader = raylib::Shader::LoadFromMemory(
    #include "../generated/skybox.vs"
    , 
    #include "../generated/skybox.fs"
            );
            cube.materials[0].shader = shader;
            shader.SetValue("environmentMap", (int)MATERIAL_MAP_CUBEMAP, SHADER_UNIFORM_INT);
        }

        void Load(std::string filename){
            if(shader.id == 0) Init();
            shader.SetValue("doGamma", 0, SHADER_UNIFORM_INT);
            shader.SetValue("vflipped", 0, SHADER_UNIFORM_INT);

            raylib::Image img(filename);
            texture.Load(img, CUBEMAP_LAYOUT_AUTO_DETECT);

            texture.SetFilter(TEXTURE_FILTER_BILINEAR);
            cube.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = texture;
        }
        
        void Draw(){
            rlDisableBackfaceCulling();
            rlDisableDepthMask();
                cube.Draw({});
            rlEnableBackfaceCulling();
            rlEnableDepthMask();
        }
    };


    struct Plane{
        //Variables for position and speed 
        float speed = 10;
        float speedTarget = 10;
        float acceleration = 30;
        raylib::Degree heading = 10;
        raylib::Degree headingTarget = 10;
        raylib::Vector3 velocity = {speed * cos(heading.RadianValue()), 0, -speed * sin(heading.RadianValue())};
        raylib::Vector3 pos = {0, 0, 0};

        //Variables for checking if keys are pressed
        bool isBackDown = false;
        bool isFrontDown = false;
        bool isRightDown = false;
        bool isLeftDown = false;
        bool isSpaceDown = false;
        bool isQDown = false;
        bool isEDown = false;        
    };

    int main(){
        //Initialize window 
        SetTargetFPS(60);
        InitWindow(800, 450, "CS381 - Assignment 3");
        
        //Loads plane model
        raylib::Model plane = LoadModel("customModel/PolyPlane.glb");
        plane.transform = raylib::Transform(plane.transform).Scale(3, 3, 3);
        plane.transform = raylib::Transform(plane.transform).RotateXYZ(0, raylib::Degree(180), 0);

        //Load tree
        raylib::Model tree = LoadModel("customModel/poly_tree.glb");
        tree.transform = raylib::Transform(tree.transform).Scale(10, 10, 10);
        tree.transform = raylib::Transform(tree.transform).RotateXYZ(raylib::Degree(-90),0,0);

        //Set up camera
        raylib::Camera camera(
            raylib::Vector3(0, 100, 800),
            raylib::Vector3(0, 0, 0),
            raylib::Vector3::Up(),
            45,
            CAMERA_PERSPECTIVE
        );
        raylib::Vector3 velocityCam = {0, 0, 0};

        //Set up sound
        InitAudioDevice();
        Sound engine = LoadSound("sound/engineSound.wav");
        bool isPlaneMoving = false;

        //Set up skybox
        Skybox skybox;
        skybox.Load("textures/skybox.png");

        //Set up ground
        auto mesh = raylib::Mesh::Plane(10'000, 10'000, 50, 50, 25);
        raylib::Model ground = ((raylib::Mesh*)&mesh)->LoadModelFrom();
        raylib::Texture grass("textures/grass.jpg");
        grass.SetFilter(TEXTURE_FILTER_BILINEAR);
        grass.SetWrap(TEXTURE_WRAP_REPEAT);
        ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;

        //Variables for position and speed 
        float speed = 10;
        float speedTarget = 10;
        float acceleration = 30;
        raylib::Degree heading = 10;
        raylib::Degree headingTarget = 10;
        raylib::Vector3 velocity = {speed * cos(heading.RadianValue()), 0, -speed * sin(heading.RadianValue())};
        raylib::Vector3 pos = {0, 0, 0};

        //Variables for checking if keys are pressed
        bool isBackDown = false;
        bool isFrontDown = false;
        bool isRightDown = false;
        bool isLeftDown = false;
        bool isSpaceDown = false;
        bool isQDown = false;
        bool isEDown = false;

        //Variable for plane switching
        int planeNum = 1;
        Plane planeOne;
        Plane planeTwo;
        Plane planeThree;

        while (!WindowShouldClose()) {
            if (IsKeyPressed(KEY_SPACE)) PlaySound(engine);
            // Inside the main loop
            if (velocity != raylib::Vector3{0, 0, 0}) {
                isPlaneMoving = true;
                //Play the sound if it's not already playing
                if (!IsSoundPlaying(engine)) {
                    PlaySound(engine);
                }
            } else {
                isPlaneMoving = false;
                //Stop the sound if it's playing
                if (IsSoundPlaying(engine)) {
                    StopSound(engine);
                }
            }

            //Camera that tracks the player, needs rework
            camera.target = pos;

            BeginDrawing();
            {   
                camera.BeginMode();
                {
                    ClearBackground(RAYWHITE);
                    skybox.Draw();
                    ground.Draw({});
                    DrawBoundedModel(plane, pos, {1, 1, 1}, {0, heading, 0});
                    DrawBoundedModel(tree, {-250, -10, 0}, {1, 1, 1}, {0, 0, 0});
                    DrawBoundedModel(tree, {300, -10, 300}, {1, 1, 1}, {0, 0, 0});

                    if(IsKeyDown(KEY_TAB)){
                        planeNum += 1;
                        planeNum %= 3;
                    }
                    switch(planeNum){
                        case 1:
                            movementControl(planeOne);
                        break;
                        case 2:
                        break;
                        case 3:
                        break;
                    };
                }
                camera.EndMode(); 
            }
            EndDrawing();
        }

        UnloadModel(plane);
        UnloadModel(tree);
        UnloadSound(engine);
        CloseAudioDevice();
    }