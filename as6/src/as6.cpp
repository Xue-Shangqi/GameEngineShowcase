#include <raylib-cpp.hpp>
#include <BufferedInput.hpp>
#include <cassert>
#include <iostream>
#include "skybox.hpp"



raylib::Transform transformer(const raylib::Transform& inputTransform) {
    return inputTransform;
}

static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
    int intPart = angle;
    float floatPart = float(angle) - intPart;
    intPart %= 360;
    intPart += (intPart < 0) * 360;
    return intPart + floatPart;
};

struct CalculateData {
    //Variables for position and speed 
    float speed = 0;
    float speedTarget = 0;
    float acceleration = 100;
    raylib::Degree heading = 10;
    raylib::Degree headingTarget = 10;
    raylib::Degree elevation = 0;
    raylib::Degree elevationTarget = 0;
    raylib::Vector3 velocity = {speed * cos(heading.RadianValue()), 0, -speed * sin(heading.RadianValue())};
    raylib::Vector3 pos = {0, 0, 0};
};

using Entity = uint8_t;
struct ComponentStorage {
	size_t elementSize = -1;
	std::vector<std::byte> data;

	ComponentStorage() : elementSize(-1), data(1, std::byte{0}) {}
	ComponentStorage(size_t elementSize) : elementSize(elementSize) {data.resize(10 * elementSize);}

	template<typename Tcomponent>
	ComponentStorage(Tcomponent reference = {}) : ComponentStorage(sizeof(Tcomponent)) {}

	template<typename Tcomponent>
	Tcomponent& Get(Entity e){
		assert(sizeof(Tcomponent) == elementSize);
		assert(e < (data.size() / elementSize));
		return *(Tcomponent*)(data.data() + e * elementSize);
	}

	template<typename Tcomponent>
	std::pair<Tcomponent&, Entity> Allocate(size_t count = 1){
		assert(sizeof(Tcomponent) == elementSize);
		assert(count < 255);
		auto originalEnd = data.size();
		data.insert(data.end(), elementSize * count, std::byte{0});
		for(size_t i = 0; i < count - 1; i++){
			new(data.data() + originalEnd + i * elementSize) Tcomponent();
		}
		return {
			*new(data.data() + data.size() - elementSize) Tcomponent(),
			data.size() / elementSize
		};
	}

	template<typename Tcomponent>
	Tcomponent& GetOrAllocate(Entity e){
		assert(sizeof(Tcomponent) == elementSize);
		size_t size = data.size() / elementSize;
		if(size <= e){
			Allocate<Tcomponent>(std::max<int64_t>(int64_t(e) - size, 1));
		}
		return Get<Tcomponent>(e);
	}
};

extern size_t globalComponentCounter;
template<typename T>
size_t GetComponentID(T reference = {}){
	static size_t id = globalComponentCounter++;
	return id;
}

struct Scene {
	std::vector<std::vector<bool>> entityMasks;
	std::vector<ComponentStorage> storages = {ComponentStorage()};
	
	template<typename Tcomponent>
	ComponentStorage& GetStorage(){
		size_t id = GetComponentID<Tcomponent>();
		if(storages.size() <= id){
			storages.insert(storages.cend(), std::max<int64_t>(id - storages.size(), 1), ComponentStorage());
		}
		if(storages[id].elementSize == std::numeric_limits<size_t>::max()){
			storages[id] = ComponentStorage(Tcomponent{});
		}
		return storages[id];
	}

	Entity CreateEntity(){
		Entity e = entityMasks.size();
		entityMasks.emplace_back(std::vector<bool>{false});
		return e;
	}

	template<typename Tcomponent>
	Tcomponent& AddComponent(Entity e){
		size_t id = GetComponentID<Tcomponent>();
		auto& eMask = entityMasks[e];
		if(eMask.size() <= id){
			eMask.resize(id + 1, false);
		}
		eMask[id] = true;
		return GetStorage<Tcomponent>().template GetOrAllocate<Tcomponent>(e);
	}

	template<typename Tcomponent>
	Tcomponent& GetComponent(Entity e){
		size_t id = GetComponentID<Tcomponent>();
		assert(entityMasks[e][id]);
		return GetStorage<Tcomponent>().template Get<Tcomponent>(e); 
	}

	template<typename Tcomponent>
	bool HasComponent(Entity e){
		size_t id = GetComponentID<Tcomponent>();
		return entityMasks.size() > e && entityMasks[e].size() > id && entityMasks[e][id];
	}
};

struct Rendering {
	raylib::Model* model;
	bool drawBoundingBox = false;
};

struct InputComponent {
	raylib::BufferedInput inputs;
	CalculateData data;
	InputComponent() : inputs(raylib::BufferedInput{}), data(CalculateData{}) {

		inputs["w"] = raylib::Action::key(KEY_W).SetPressedCallback([&]{
			data.speedTarget += 20;
		}).move();

		inputs["s"] = raylib::Action::key(KEY_S).SetPressedCallback([&]{
			data.speedTarget -= 20;
		}).move();
    }

	void PollEvents(){
		inputs.PollEvents();
	}
};

void movementControl(Entity& entity, Scene& scene);
void movementCalc(Entity& entity, Scene& scene);
void Draw(Scene& scene);
void createModel(Entity& entity, raylib::Model& model, Scene& scene, raylib::Vector3 pos);

int main() {
	// Create window
	const int screenWidth = 800 * 1.5;
	const int screenHeight = 450 * 1.5;
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 6");

	// Create camera
	auto camera = raylib::Camera(
		raylib::Vector3(0, 120, -500), 
		raylib::Vector3(0, 0, 300), 
		raylib::Vector3::Up(), 
		45.0f,
		CAMERA_PERSPECTIVE
	);

	// Create skybox
	cs381::SkyBox skybox("textures/skybox.png");

	// Create ground
	auto mesh = raylib::Mesh::Plane(10000, 10000, 50, 50, 25);
	raylib::Model ground = ((raylib::Mesh*)&mesh)->LoadModelFrom();
	raylib::Texture water("textures/water.jpg");
	water.SetFilter(TEXTURE_FILTER_BILINEAR);
	water.SetWrap(TEXTURE_WRAP_REPEAT);
	ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;

	// Create airplane
	raylib::Model plane("customModel/PolyPlane.glb");

	// Create Ships
	raylib::Model cargo("customModel/CargoG_HOSBrigadoon.glb");
	cargo.transform = raylib::Transform(cargo.transform).Scale(0.025, 0.025, 0.025).RotateXYZ(raylib::Degree(90), raylib::Degree(0), raylib::Degree(90));
	raylib::Model ship("customModel/ddg51.glb");
	ship.transform = raylib::Transform(ship.transform).RotateXYZ(raylib::Degree(90), raylib::Degree(0), raylib::Degree(-90));
	raylib::Model oilTanker ("customModel/OilTanker.glb");
	oilTanker.transform = raylib::Transform(oilTanker.transform).Scale(0.006, 0.006, 0.006).RotateXYZ(raylib::Degree(90), raylib::Degree(0), raylib::Degree(90));
	raylib::Model explorer("customModel/OrientExplorer.glb");
	explorer.transform = raylib::Transform(explorer.transform).Scale(0.015, 0.015, 0.015).RotateXYZ(raylib::Degree(90), raylib::Degree(0), raylib::Degree(90));
	raylib::Model smit("customModel/SmitHouston_Tug.glb");
	smit.transform = raylib::Transform(smit.transform).Scale(1.5, 1.5, 1.5).RotateXYZ(raylib::Degree(0), raylib::Degree(90), raylib::Degree(0));

	// Create the scene
	Scene scene;
	raylib::BufferedInput inputs;
	
	// Entity for planes
	Entity pEntityOne = scene.CreateEntity();
	Entity pEntityTwo = scene.CreateEntity();
	Entity pEntityThree = scene.CreateEntity();
	Entity pEntityFour = scene.CreateEntity();
	Entity pEntityFive = scene.CreateEntity();

	// Entity for ships
	Entity sEntityOne = scene.CreateEntity();
	Entity sEntityTwo = scene.CreateEntity();
	Entity sEntityThree = scene.CreateEntity();
	Entity sEntityFour = scene.CreateEntity();
	Entity sEntityFive = scene.CreateEntity();

	// Creates the five planes
	createModel(pEntityOne, plane, scene, {0, 80, 0});
	createModel(pEntityTwo, plane, scene, {150, 80, 0});
	createModel(pEntityThree, plane, scene, {300, 80, 0});
	createModel(pEntityFour, plane, scene, {-150, 80, 0});
	createModel(pEntityFive, plane, scene, {-300, 80, 0});

	// Creates the five ship
	createModel(sEntityOne, cargo, scene, {-300, 0, 0});
	createModel(sEntityTwo, ship, scene, {-150, 0, 0});
	createModel(sEntityThree, smit, scene, {0, 0, 0});
	createModel(sEntityFour, explorer, scene, {150, 0, 0});
	createModel(sEntityFive, oilTanker, scene, {300, 0, 0});

	// Main loop
	bool keepRunning = true;
	while(!window.ShouldClose() && keepRunning) {
		// Updates
		if(IsKeyPressed(KEY_ESCAPE)){
			keepRunning = false;
		}
		// Rendering
		window.BeginDrawing();
		{
			// Clear screen
			window.ClearBackground(BLACK);

			camera.BeginMode();
			{
				// Render skybox and ground
				skybox.Draw();
				ground.Draw({});

				movementCalc(pEntityOne, scene);
				InputComponent& inputComponent = scene.GetComponent<InputComponent>(pEntityOne);
				inputComponent.PollEvents();
				raylib::Transform& planeOne = scene.GetComponent<raylib::Transform>(pEntityOne);
				planeOne = planeOne.Translate(inputComponent.data.pos).RotateY(raylib::Degree(inputComponent.data.heading));
				Draw(scene);
			}
			camera.EndMode();

			// Measure our FPS
			DrawFPS(10, 10);
		}
		window.EndDrawing();
	}

	return 0;
}

void movementCalc(Entity& entity, Scene& scene) {
	InputComponent& inputComponent = scene.GetComponent<InputComponent>(entity);
    //Speed calculation
    if(inputComponent.data.speedTarget > inputComponent.data.speed){
        inputComponent.data.speed += inputComponent.data.acceleration * GetFrameTime();
    }else if(inputComponent.data.speedTarget < inputComponent.data.speed){
        inputComponent.data.speed -= inputComponent.data.acceleration * GetFrameTime();
    }
    inputComponent.data.velocity = raylib::Vector3{-inputComponent.data.speed * cos(inputComponent.data.heading.RadianValue()), inputComponent.data.speed * sin(inputComponent.data.elevation.RadianValue()), inputComponent.data.speed * sin(inputComponent.data.heading.RadianValue())};
    inputComponent.data.pos += inputComponent.data.velocity * GetFrameTime();
    

    //Heading calculation
    inputComponent.data.headingTarget = AngleClamp(inputComponent.data.headingTarget);
    inputComponent.data.heading = AngleClamp(inputComponent.data.heading);
    float difference = abs(inputComponent.data.headingTarget - inputComponent.data.heading);
    if(inputComponent.data.headingTarget > inputComponent.data.heading){
        if(difference < 180) inputComponent.data.heading += inputComponent.data.acceleration * GetFrameTime();
        else if(difference > 180) inputComponent.data.heading -= inputComponent.data.acceleration * GetFrameTime();
    }else if(inputComponent.data.headingTarget < inputComponent.data.heading){
        if(difference < 180) inputComponent.data.heading -= inputComponent.data.acceleration * GetFrameTime();
        else if(difference > 180) inputComponent.data.heading += inputComponent.data.acceleration * GetFrameTime();
    }

    //Elevation calculation
    if(inputComponent.data.elevationTarget > inputComponent.data.elevation){
        inputComponent.data.elevation += inputComponent.data.acceleration * GetFrameTime();
    }else if(inputComponent.data.elevationTarget < inputComponent.data.elevation){
        inputComponent.data.elevation -= inputComponent.data.acceleration * GetFrameTime();
    }
}

void Draw(Scene& scene) {
	for(Entity e = 0; e < scene.entityMasks.size(); e++){
		if(!scene.HasComponent<Rendering>(e)) continue;
		
		auto& rendering = scene.GetComponent<Rendering>(e);

		raylib::Transform backupTransform = rendering.model->transform;
		rendering.model->transform = scene.GetComponent<raylib::Transform>(e);
		rendering.model->Draw({});
		rendering.model->transform = backupTransform;
		}
}

void createModel(Entity& entity, raylib::Model& model, Scene& scene, raylib::Vector3 pos){
	Rendering& renderingComponent = scene.AddComponent<Rendering>(entity);
	renderingComponent.model = &model;
	raylib::Transform& transform = scene.AddComponent<raylib::Transform>(entity);
	transform = renderingComponent.model->transform;
	transform = transform.Translate(pos);


	InputComponent& inputComponent  = scene.AddComponent<InputComponent>(entity);
}
