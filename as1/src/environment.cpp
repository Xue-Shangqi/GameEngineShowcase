#include "raylib-cpp.hpp"
#include "rlgl.h"

struct Skybox {
    raylib::Texture texture;
    raylib::Shader shader;
    raylib::Model cube;

    Skybox(): shader(0) {};

    void init(){
        auto gen = raylib::Mesh::Cube(1, 1, 1);
        cube = ((raylib::Mesh*)(&gen))->LoadModelFrom();

        shader = raylib::Shader::LoadFromMemory(vertexShader, fragmentShader);
        cube.materials[0].shader = shader;
        shader.SetValue("environmentMap", (int)MATERIAL_MAP_CUBEMAP, SHADER_UNIFORM_INT);
    }

    void Load(std::string fileName){
        shader.SetValue("doGamma", 0, SHADER_UNIFORM_INT);
        shader.SetValue("vFilpped", 0, SHADER_UNIFORM_INT);

        raylib::Image img(fileName);
        texture.Load(fileName);
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
    raylib::Window window(800, 400, "CS381 - Assignment 0");

    raylib::Model bad("bad.obj");
    raylib::Camera camera{
        raylib::Vector3(0, 120, 500),
        raylib::Vector3(0, 0, 300),
        raylib::Vector3::Up,
        CAMERA_PERSPECTIVE
    }
}