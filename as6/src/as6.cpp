#include <raylib-cpp.hpp>
#include <cassert>
#include "counter.cpp"
#include "skybox.hpp"

bool ProcessInput(raylib::Degree& planeTargetHeading, float& planeTargetSpeed, size_t& selectedPlane);
raylib::Vector3 CaclulateVelocity(const CalculateVelocityParams& data);
void DrawBoundedModel(raylib::Model& model, Transformer auto transformer);
void DrawModel(raylib::Model& model, Transformer auto transformer);

template<typename T>
concept Transformer = requires(T t, raylib::Transform m) {
	{ t.operator()(m) } -> std::convertible_to<raylib::Transform>;
};

raylib::Transform transformer(const raylib::Transform& inputTransform) {
    return inputTransform;
}

struct CalculateVelocityParams {
	static constexpr float acceleration = 5;
	static constexpr float angularAcceleration = 60;

	float targetSpeed;
	raylib::Degree targetHeading;
	float& speed;
	raylib::Degree& heading;
	float dt;

	float maxSpeed = 50;
	float minSpeed = 0;
};

using Entity = uint8_t;
struct ComponentStorage {
	size_t elementSize = -1;
	std::vector<std::byte> data;

	ComponentStorage() : elementSize(-1), data(1, std::byte{0}) {}
	ComponentStorage(size_t elementSize) : elementSize(elementSize) {data.resize(5 * elementSize);}

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

void Draw(Scene& scene) {
	for(Entity e = 0; e < scene.entityMasks.size(); e++){
		if(!scene.HasComponent<Rendering>(e)) continue;
		
		auto& rendering = scene.GetComponent<Rendering>(e);

		raylib::Transform backupTransform = rendering.model->transform;
		rendering.model->transform = transformer(backupTransform);
		rendering.model->Draw({});
		rendering.model->transform = backupTransform;
		}
};

int main() {
	// Create window
	const int screenWidth = 800 * 2;
	const int screenHeight = 450 * 2;
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
	raylib::Model ship("customModel/ddg51.glb");
	raylib::Model oilTanker("customModel/OilTanker.glb");
	raylib::Model explorer("customModel/OrientExplorer.glb");
	raylib::Model smit("customModel/SmitHouston_Tug.glb");

	// Main loop
	bool keepRunning = true;
	while(!window.ShouldClose() && keepRunning) {
		// Updates

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


			}
			camera.EndMode();

			// Measure our FPS
			DrawFPS(10, 10);
		}
		window.EndDrawing();
	}

	return 0;
}

// Input handling
bool ProcessInput(raylib::Degree& planeTargetHeading, float& planeTargetSpeed, size_t& selectedPlane) {


	return true;
}

raylib::Vector3 CaclulateVelocity(const CalculateVelocityParams& data) {
	static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
		float decimal = float(angle) - int(angle);
		int whole = int(angle) % 360;
		whole += (whole < 0) * 360;
		return decimal + whole;
	};

	float target = Clamp(data.targetSpeed, data.minSpeed, data.maxSpeed);
	if(data.speed < target) data.speed += data.acceleration * data.dt;
	else if(data.speed > target) data.speed -= data.acceleration * data.dt;
	data.speed = Clamp(data.speed, data.minSpeed, data.maxSpeed);

	target = AngleClamp(data.targetHeading);
	float difference = abs(target - data.heading);
	if(target > data.heading) {
		if(difference < 180) data.heading += data.angularAcceleration * data.dt;
		else if(difference > 180) data.heading -= data.angularAcceleration * data.dt;
	} else if(target < data.heading) {
		if(difference < 180) data.heading -= data.angularAcceleration * data.dt;
		else if(difference > 180) data.heading += data.angularAcceleration * data.dt;
	} 
	if(difference < .5) data.heading = target; // If the heading is really close to correct 
	data.heading = AngleClamp(data.heading);
	raylib::Radian angle = raylib::Degree(data.heading);

	return {cos(angle) * data.speed, 0, -sin(angle) * data.speed};
}

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
	raylib::Transform backupTransform = model.transform;
	model.transform = transformer(backupTransform);
	model.Draw({});
	model.GetTransformedBoundingBox().Draw();
	model.transform = backupTransform;
}

void DrawModel(raylib::Model& model, Transformer auto transformer) {
	raylib::Transform backupTransform = model.transform;
	model.transform = transformer(backupTransform);
	model.Draw({});
	model.transform = backupTransform;
}