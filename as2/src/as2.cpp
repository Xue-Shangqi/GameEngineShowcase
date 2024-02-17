#include "raylib-cpp.hpp"
#include "rlgl.h"

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
    raylib::Vector3 pos = {0, 0, 0};
    raylib::Vector3 velocity = {0, 0, 0};

    //Variables for checking if keys are pressed
    bool isWDown = false;
    bool isSDown = false;
    bool isADown = false;
    bool isDDown = false;
    bool isQDown = false;
    bool isEDown = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        {   
            camera.BeginMode();
            {
                ClearBackground(WHITE);
                skybox.Draw();
                ground.Draw({});
                DrawBoundedModel(plane, pos, {1, 1, 1}, {0, 0, 0});

                //Movement conditions
                if(IsKeyDown(KEY_W) && !isWDown){
                    velocity += raylib::Vector3::Back() * speed;
                }
                isWDown = IsKeyDown(KEY_W);

                if(IsKeyDown(KEY_S) && !isSDown){
                    velocity += raylib::Vector3::Forward() * speed;
                }
                isSDown = IsKeyDown(KEY_S);

                if(IsKeyDown(KEY_A) && !isADown){
                    velocity += raylib::Vector3::Right() * speed;
                }
                isADown = IsKeyDown(KEY_A);

                if(IsKeyDown(KEY_D) && !isDDown){
                    velocity += raylib::Vector3::Left() * speed;
                }
                isDDown = IsKeyDown(KEY_D);

                if(IsKeyDown(KEY_Q) && !isQDown){
                    velocity += raylib::Vector3::Up() * speed;
                }
                isQDown = IsKeyDown(KEY_Q);

                if(IsKeyDown(KEY_E) && !isEDown){
                    velocity += raylib::Vector3::Down() * speed;
                }
                isEDown = IsKeyDown(KEY_E);

                pos += velocity * speed * GetFrameTime();

            }
            camera.EndMode(); 
        }
        EndDrawing();
    }

    UnloadModel(plane);
}