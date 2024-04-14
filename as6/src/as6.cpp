#include <raylib-cpp.hpp>
#include <BufferedInput.hpp>
#include <cassert>
#include <iostream>
#include "skybox.hpp"



raylib::Transform transformer(const raylib::Transform& inputTransform) {
    return inputTransform;
}

static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
		float decimal = float(angle) - int(angle);
		int whole = int(angle) % 360;
		whole += (whole < 0) * 360;
		return decimal + whole;
};


struct CalculateData {
    // Variables for position and speed 
    float speed = 0;
    float speedTarget = 0;
    float acceleration = 50;
    float turningRate = 70;
	float maxSpeed = 90;
    raylib::Degree heading = 0;
    raylib::Degree headingTarget = 0;
    raylib::Degree elevation = 0;
    raylib::Degree elevationTarget = 0;
    raylib::Vector3 velocity {
        speed * cos(heading.RadianValue()),
        0,  // Assuming y-axis is vertical
        -speed * sin(heading.RadianValue())
    };
    raylib::Vector3 pos {0, 0, 0};
	bool moving = false;
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

void Draw(Scene& scene);
void createModel(Entity& entity, raylib::Model& model, Scene& scene, raylib::Vector3 pos);
void Update(Scene& scene, raylib::BufferedInput& inputs, int selected);
void ModifyData(Entity& entity, Scene& scene, float maxSpeed, float acceleration, float turnRate);

// Input component that takes care of user inputs using a buffered input system
struct InputComponent {
	void PollEvent(raylib::BufferedInput& inputs){
		inputs.PollEvents();
	}

	void ControlMovement(raylib::BufferedInput& inputs, CalculateData& data){

		// The if state in each lambda function checks if the key is only pressed one
		// because the code would run multiple times if the statement is not there 
		// since it runs per frame
		inputs["w"] = raylib::Action::key(KEY_W).SetPressedCallback([&]{
			if(IsKeyPressed(KEY_W)){
				data.speedTarget += 20;
				data.moving = true;
			}
		}).move();

		inputs["s"] = raylib::Action::key(KEY_S).SetPressedCallback([&]{
			if(IsKeyPressed(KEY_S)){
				data.speedTarget -= 20;
				data.moving = true;
			}
		}).move();

		inputs["a"] = raylib::Action::key(KEY_A).SetPressedCallback([&]{
			if(IsKeyPressed(KEY_A)){
				data.headingTarget += 30;
			}
		}).move();

		inputs["d"] = raylib::Action::key(KEY_D).SetPressedCallback([&]{
			if(IsKeyPressed(KEY_D)){
				data.headingTarget -= 30;
			}
		}).move();

		inputs["space"] = raylib::Action::key(KEY_SPACE).SetPressedCallback([&]{
			if(IsKeyPressed(KEY_SPACE)){
				data.speedTarget = 0;
				data.moving = false;
			}
		}).move();
	}
};

// Transform component storing the original transform when they are first created and
// the transform used for calculation and rendering
struct TransformComponent {
	raylib::Transform ogTransform = raylib::Transform();
	raylib::Transform newTransform = raylib::Transform();
	TransformComponent() : ogTransform(), newTransform() {}
};

// Combined both phsyics and rendering component into one huge render component struct with all the 
// physics calulation and setting the transform for rendering.
struct RenderComponent {
	void movementCalc(Entity& entity, Scene& scene) {
		CalculateData& data = scene.GetComponent<CalculateData>(entity);
		//Speed calculation
		if(data.speedTarget > data.speed){
			data.speed += data.acceleration * GetFrameTime();
		}else if(data.speedTarget < data.speed || data.maxSpeed < data.speed){
			data.speed -= data.acceleration * GetFrameTime();
		}
		data.velocity = raylib::Vector3{data.speed * cos(data.heading.RadianValue()), data.speed * sin(data.elevation.RadianValue()), -data.speed * sin(data.heading.RadianValue())};
		data.pos += data.velocity * GetFrameTime();

		//Heading calculation
		// printf("Target: %f Heading: %f \n", (float)data.headingTarget, (float)data.heading);
		data.headingTarget = AngleClamp(data.headingTarget);
		data.heading = AngleClamp(data.heading);
		float difference = abs(data.headingTarget - data.heading);
		if(data.headingTarget > data.heading){
			if(difference < 180) data.heading += data.turningRate * GetFrameTime();
			else if(difference > 180) data.heading -= data.turningRate * GetFrameTime();
		}else if(data.headingTarget < data.heading){
			if(difference < 180) data.heading -= data.turningRate * GetFrameTime();
			else if(difference > 180) data.heading += data.turningRate * GetFrameTime();
		}

		//Elevation calculation (did not use)
		if(data.elevationTarget > data.elevation){
			data.elevation += data.acceleration * GetFrameTime();
		}else if(data.elevationTarget < data.elevation){
			data.elevation -= data.acceleration * GetFrameTime();
		}
	}

	void UpdateMovement(Entity& entity, Scene& scene, raylib::BufferedInput& inputs){
		movementCalc(entity, scene);
		InputComponent& inputComponent = scene.GetComponent<InputComponent>(entity);
		CalculateData& data = scene.GetComponent<CalculateData>(entity);
		Rendering& render = scene.GetComponent<Rendering>(entity);

		inputComponent.PollEvent(inputs);
		if(render.drawBoundingBox){
			inputComponent.ControlMovement(inputs, data);
		}

		TransformComponent& transform = scene.GetComponent<TransformComponent>(entity);
		transform.newTransform = transform.ogTransform.Translate(data.pos).RotateY(data.heading);
	}
};



int main() {
	// Create window
	const int screenWidth = 800 * 1.5;
	const int screenHeight = 450 * 1.5;
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 6");
	window.SetTargetFPS(60);

	// Create camera
	auto camera = raylib::Camera(
        raylib::Vector3(0, 200, -500),
        raylib::Vector3(0, 0, 0),
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

	// Create audio 
    InitAudioDevice();
    Sound wind = LoadSound("sound/wind.mp3");

	// Create plane
	raylib::Model plane("customModel/PolyPlane.glb");
	raylib::Model space("customModel/spaceship.glb");
	space.transform = raylib::Transform(space.transform).Scale(4, 4, 4).RotateY(raylib::Degree(90));

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
	createModel(pEntityOne, space, scene, {0, 80, 0});
	ModifyData(pEntityOne, scene, 200.0, 100.0, 200.0);
	createModel(pEntityTwo, plane, scene, {150, 80, 0});
	createModel(pEntityThree, plane, scene, {300, 80, 0});
	createModel(pEntityFour, plane, scene, {-150, 80, 0});
	createModel(pEntityFive, plane, scene, {-300, 80, 0});

	// Creates the five ship
	createModel(sEntityOne, cargo, scene, {-300, 0, 0});
	ModifyData(sEntityOne, scene, 30.0, 20.0, 25.0);
	createModel(sEntityTwo, ship, scene, {-150, 0, 0});
	ModifyData(sEntityTwo, scene, 50.0, 40.0, 30.0);
	createModel(sEntityThree, smit, scene, {0, 0, 0});
	ModifyData(sEntityThree, scene, 40.0, 50.0, 35.0);
	createModel(sEntityFour, explorer, scene, {150, 0, 0});
	ModifyData(sEntityFour, scene, 60.0, 50.0, 40.0);
	createModel(sEntityFive, oilTanker, scene, {300, 0, 0});
	ModifyData(sEntityFive, scene, 35.0, 25.0, 30.0);

	// Creates tracker for which plane is selected
	int selected = 0;

	// Main loop
	bool keepRunning = true;
	while(!window.ShouldClose() && keepRunning) {
		// Quit os escape is pressed
		if(IsKeyPressed(KEY_ESCAPE)){
			keepRunning = false;
		}

		// Selection system
		if(IsKeyPressed(KEY_TAB)){
			selected ++;
			if (selected > 9) {
				selected = 0;
			}
		}

		// Sound system
		CalculateData& dataOne = scene.GetComponent<CalculateData>(pEntityOne);
		CalculateData& dataTwo = scene.GetComponent<CalculateData>(pEntityTwo);
		CalculateData& dataThree = scene.GetComponent<CalculateData>(pEntityThree);
		CalculateData& dataFour = scene.GetComponent<CalculateData>(pEntityFour);
		CalculateData& dataFive = scene.GetComponent<CalculateData>(pEntityFive);
		
		if(dataOne.moving || dataTwo.moving || dataThree.moving || dataFour.moving || dataFive.moving){
			if (!IsSoundPlaying(wind)) {
                PlaySound(wind);
            }
		} else {
            if (IsSoundPlaying(wind)) {
                StopSound(wind);
            }
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

				
				Update(scene, inputs, selected);
				Draw(scene);
			}
			camera.EndMode();

			// Measure our FPS
			DrawFPS(10, 10);
		}
		window.EndDrawing();
	}

	CloseAudioDevice();
	return 0;
}

// Draws the whole scene (i.e. all models)
void Draw(Scene& scene) {
	for(Entity e = 0; e < scene.entityMasks.size(); e++){
		if(!scene.HasComponent<Rendering>(e)) continue;
		
		auto& rendering = scene.GetComponent<Rendering>(e);

		TransformComponent& transform = scene.GetComponent<TransformComponent>(e);
		raylib::Transform backupTransform = rendering.model->transform;
		rendering.model->transform = transform.newTransform;
		rendering.model->Draw({});
		if(rendering.drawBoundingBox){
			rendering.model->GetTransformedBoundingBox().Draw();
		}
		rendering.model->transform = backupTransform;
		transform.newTransform = transform.ogTransform;
	}
}

// Updates the calcualtion for all models in the scene
void Update(Scene& scene, raylib::BufferedInput& inputs, int selected){
	for(Entity e = 0; e < scene.entityMasks.size(); e++){
		if(!scene.HasComponent<RenderComponent>(e)) continue;
	
		RenderComponent& renderComponent = scene.GetComponent<RenderComponent>(e);
		CalculateData& data = scene.GetComponent<CalculateData>(e);
		Rendering& render = scene.GetComponent<Rendering>(e);

		// Used to update the selected object
		if(selected == e){
			render.drawBoundingBox = true;
		}else{
			render.drawBoundingBox = false;
		}


		// Updates the movement and rotation
		renderComponent.UpdateMovement(e, scene, inputs);
	}
}

// Initial the model with all the components they need
void createModel(Entity& entity, raylib::Model& model, Scene& scene, raylib::Vector3 pos){
	Rendering& renderingComponent = scene.AddComponent<Rendering>(entity);
	renderingComponent.model = &model;

	TransformComponent& transform = scene.AddComponent<TransformComponent>(entity);
	transform.ogTransform = renderingComponent.model->transform;
	transform.ogTransform = transform.ogTransform.Translate(pos);

	// Create the input component to process inputs
	InputComponent& inputComponent  = scene.AddComponent<InputComponent>(entity);

	// Crates the data to use for movements
	CalculateData& data = scene.AddComponent<CalculateData>(entity);
	CalculateData holder = CalculateData();
	data = holder;

	// Creates the rendering component
	RenderComponent& renderComponent = scene.AddComponent<RenderComponent>(entity);
}

// Modifies the data of different entity
void ModifyData(Entity& entity, Scene& scene, float maxSpeed, float acceleration, float turnRate){
	CalculateData& data = scene.GetComponent<CalculateData>(entity);

	data.maxSpeed = maxSpeed;
	data.acceleration = acceleration;
	data.turningRate = turnRate;
}