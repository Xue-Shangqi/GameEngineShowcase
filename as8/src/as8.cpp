#include <raylib-cpp.hpp>
#include <BufferedInput.hpp>
#include "skybox.hpp"
#include "ECS.hpp"

static constexpr auto AngleClamp = [](raylib::Degree angle) -> raylib::Degree {
		float decimal = float(angle) - int(angle);
		int whole = int(angle) % 360;
		whole += (whole < 0) * 360;
		return decimal + whole;
};

// Components
struct Rendering {
    raylib::Model* model;
    bool drawBoundingBox = false;
};

struct TransformComp {
	raylib::Transform ogTransform = raylib::Transform();
	raylib::Transform newTransform = raylib::Transform();
};

struct Velocity {
	float speed = 0;
	float acceleration = GetRandomValue(50, 80);
	float maxSpeed = GetRandomValue(80, 120);
	raylib::Degree target = 0;
	raylib::Vector3 velocity {0, 0, 0};
	raylib::Vector3 pos {0, 0, 0};
};

struct TwoD {
	raylib::Degree heading = 0;
	raylib::Degree targetHeading = 0;
	float turningRate = GetRandomValue(50, 100);
};

struct ThreeD {
	float angularAcc = 1;
	raylib::Quaternion rotation = raylib::Quaternion({});
	raylib::Quaternion targetRotation = raylib::Vector4{.1, 0, 0, 0};
};

void InputManager(raylib::BufferedInput&, Scene<ComponentStorage>&, Entity&);
void DrawBoundedModel(raylib::Model&, TransformComp&);
void DrawModel(raylib::Model&, TransformComp&);
void SetupModel(Scene<ComponentStorage>&, raylib::Model&, raylib::Vector3);
void calcHeading2D(TwoD&);
void Update(Scene<ComponentStorage>&);

void DrawSystem(Scene<ComponentStorage>& scene){
    for(Entity e = 0; e < scene.entityMasks.size(); e++) {
        if(!scene.HasComponent<Rendering>(e)) continue;
		if(!scene.HasComponent<TransformComp>(e)) continue;
        auto& rendering = scene.GetComponent<Rendering>(e);
        auto& transformComp = scene.GetComponent<TransformComp>(e);
        

        if(rendering.drawBoundingBox)
            DrawBoundedModel(*rendering.model, transformComp);
        else DrawModel(*rendering.model, transformComp);
    }
}

void Update(Scene<ComponentStorage>& scene){
	// Does calculation on entity if there is a change in their 2D/3D component and velocity
    for(Entity e = 0; e < scene.entityMasks.size(); e++) {
		auto& velData = scene.GetComponent<Velocity>(e);
		if(velData.target > velData.speed){
			velData.speed += velData.acceleration * GetFrameTime();
		}else if(velData.target < velData.speed || velData.maxSpeed < velData.speed){
			velData.speed -= velData.acceleration * GetFrameTime();
		}

		// 2D math
        if(scene.HasComponent<TwoD>(e) && scene.HasComponent<TransformComp>(e)){
			auto& data = scene.GetComponent<TwoD>(e);
			auto& transform = scene.GetComponent<TransformComp>(e);
			calcHeading2D(data);			
			velData.velocity = raylib::Vector3{velData.speed * cos(data.heading.RadianValue()), 0, -velData.speed * sin(data.heading.RadianValue())};
			velData.pos += velData.velocity * GetFrameTime();			
			transform.newTransform = transform.ogTransform.RotateY(data.heading).Translate(velData.pos);
		}else{
			// 3D math
			auto& data = scene.GetComponent<ThreeD>(e);
			auto& transform = scene.GetComponent<TransformComp>(e);

			data.rotation = data.rotation.Slerp(data.targetRotation, data.angularAcc);
			raylib::Quaternion buffer = 10;
			velData.velocity = raylib::Vector3::Left().RotateByQuaternion(data.rotation * buffer) * velData.speed;
			velData.pos += velData.velocity * GetFrameTime();
			std::pair<Vector3, raylib::Radian> pair = data.rotation.ToAxisAngle();
			transform.newTransform = raylib::Transform(transform.ogTransform).Rotate(pair.first, pair.second).Translate(velData.pos);
		}
    }
}

void InputManager(raylib::BufferedInput& inputs, Scene<ComponentStorage>& scene, Entity& selected){
	inputs["w"] = raylib::Action::key(KEY_W).SetPressedCallback([&]{
		auto& velData = scene.GetComponent<Velocity>(selected);
		velData.target += 20;
	}).move();

	inputs["s"] = raylib::Action::key(KEY_S).SetPressedCallback([&]{
		auto& velData = scene.GetComponent<Velocity>(selected);
		velData.target -= 20;
	}).move();

	inputs["a"] = raylib::Action::key(KEY_A).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(15));
		}else{
			auto& data = scene.GetComponent<TwoD>(selected);
			data.targetHeading += 30;
		}
	}).move();

	inputs["d"] = raylib::Action::key(KEY_D).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(-15));
		}else{
			auto& data = scene.GetComponent<TwoD>(selected);
			data.targetHeading -= 30;
		}
	}).move();

	inputs["q"] = raylib::Action::key(KEY_Q).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Left().RotateByQuaternion(data.rotation), raylib::Degree(15));
		}
	}).move();

	inputs["e"] = raylib::Action::key(KEY_E).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Left().RotateByQuaternion(data.rotation), raylib::Degree(-15));
		}
	}).move();
	inputs["R"] = raylib::Action::key(KEY_R).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Forward().RotateByQuaternion(data.rotation), raylib::Degree(15));
		}
	}).move();

	inputs["F"] = raylib::Action::key(KEY_F).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Forward().RotateByQuaternion(data.rotation), raylib::Degree(-15));
		}
	}).move();

	inputs["space"] = raylib::Action::key(KEY_SPACE).SetPressedCallback([&]{
		auto& velData = scene.GetComponent<Velocity>(selected);
		velData.target = 0;
	}).move();
}



void DrawBoundedModel(raylib::Model& model, TransformComp& transform){
	raylib::Transform backupTransform = model.transform;
	model.transform = transform.newTransform;
	model.Draw({});
	model.GetTransformedBoundingBox().Draw();
	model.transform = backupTransform;
	transform.newTransform = transform.ogTransform;
}

void DrawModel(raylib::Model& model, TransformComp& transform){
	raylib::Transform backupTransform = model.transform;
	model.transform = transform.newTransform;
	model.Draw({});
	model.transform = backupTransform;
	transform.newTransform = transform.ogTransform;
}

void SetupModel(Scene<ComponentStorage>& scene, raylib::Model& model, raylib::Vector3 start){
	Entity e = scene.CreateEntity();
	auto& rendering = scene.AddComponent<Rendering>(e);
	rendering.model = &model;

	auto& transform = scene.AddComponent<TransformComp>(e);
	transform.ogTransform = rendering.model->transform;
	transform.ogTransform = transform.ogTransform.Translate(start);
	transform.newTransform = transform.ogTransform;
}

void calcHeading2D(TwoD& data){
	data.targetHeading = AngleClamp(data.targetHeading);
	data.heading = AngleClamp(data.heading);
	float difference = abs(data.targetHeading - data.heading);
	if(data.targetHeading > data.heading){
		if(difference < 180) data.heading += data.turningRate * GetFrameTime();
		else if(difference > 180) data.heading -= data.turningRate * GetFrameTime();
	}else if(data.targetHeading < data.heading){
		if(difference < 180) data.heading -= data.turningRate * GetFrameTime();
		else if(difference > 180) data.heading += data.turningRate * GetFrameTime();
	}
}


int main(){
	// Create window
	const int screenWidth = 800 * 1.5;
	const int screenHeight = 450 * 1.5;
	raylib::Window window(screenWidth, screenHeight, "CS381 - Assignment 8");
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
	SkyBox skybox("textures/skybox.png");

	// Create ground
	auto mesh = raylib::Mesh::Plane(10000, 10000, 50, 50, 25);
	raylib::Model ground = ((raylib::Mesh*)&mesh)->LoadModelFrom();
	raylib::Texture water("textures/water.jpg");
	water.SetFilter(TEXTURE_FILTER_BILINEAR);
	water.SetWrap(TEXTURE_WRAP_REPEAT);
	ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;



    // Create the scene
    Scene scene = Scene();
    raylib::BufferedInput inputs;

	// Import models
	raylib::Model plane("customModel/PolyPlane.glb");
	plane.transform = raylib::Transform(plane.transform).RotateXYZ(raylib::Degree(180), 0, 0);
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

    // Create the five ships and plane
	SetupModel(scene, plane, {0, 80, 0});
	SetupModel(scene, plane, {150, 80, 0});
	SetupModel(scene, plane, {300, 80, 0});
	SetupModel(scene, plane, {-150, 80, 0});
	SetupModel(scene, plane, {-300, 80, 0});

	SetupModel(scene, cargo, {-300, 0, 0});
	SetupModel(scene, ship, {-150, 0, 0});
	SetupModel(scene, smit, {0, 0, 0});
	SetupModel(scene, explorer, {150, 0, 0});
	SetupModel(scene, oilTanker, {300, 0, 0});
	// SetupModel(scene, oilTanker, {300, 0, 0});

	// Creates tracker for which plane is selected
	Entity selected = 0;
	auto& renderingStart = scene.GetComponent<Rendering>(selected);
	renderingStart.drawBoundingBox = true;

	for(Entity i = 0; i < scene.entityMasks.size(); i++){
		scene.AddComponent<Velocity>(i);
		if(i < 5) {scene.AddComponent<ThreeD>(i);}
		else {scene.AddComponent<TwoD>(i);}
	}
	
	inputs["w"] = raylib::Action::key(KEY_W).SetPressedCallback([&]{
		auto& velData = scene.GetComponent<Velocity>(selected);
		velData.target += 20;
	}).move();

	inputs["s"] = raylib::Action::key(KEY_S).SetPressedCallback([&]{
		auto& velData = scene.GetComponent<Velocity>(selected);
		velData.target -= 20;
	}).move();

	inputs["a"] = raylib::Action::key(KEY_A).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(15));
		}else{
			auto& data = scene.GetComponent<TwoD>(selected);
			data.targetHeading += 30;
		}
	}).move();

	inputs["d"] = raylib::Action::key(KEY_D).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Up(), raylib::Degree(-15));
		}else{
			auto& data = scene.GetComponent<TwoD>(selected);
			data.targetHeading -= 30;
		}
	}).move();

	inputs["q"] = raylib::Action::key(KEY_Q).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Left().RotateByQuaternion(data.rotation), raylib::Degree(15));
		}
	}).move();

	inputs["e"] = raylib::Action::key(KEY_E).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Left().RotateByQuaternion(data.rotation), raylib::Degree(-15));
		}
	}).move();
	inputs["R"] = raylib::Action::key(KEY_R).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Forward().RotateByQuaternion(data.rotation), raylib::Degree(15));
		}
	}).move();

	inputs["F"] = raylib::Action::key(KEY_F).SetPressedCallback([&]{
		if(selected < 5){
			auto& data = scene.GetComponent<ThreeD>(selected);
			data.targetRotation = data.targetRotation * raylib::Quaternion::FromAxisAngle(raylib::Vector3::Forward().RotateByQuaternion(data.rotation), raylib::Degree(-15));
		}
	}).move();

	inputs["space"] = raylib::Action::key(KEY_SPACE).SetPressedCallback([&]{
		auto& velData = scene.GetComponent<Velocity>(selected);
		velData.target = 0;
	}).move();


	
	bool keepRunning = true;
	while(!window.ShouldClose() && keepRunning) {
		// Quit if escape is pressed
		if(IsKeyPressed(KEY_ESCAPE)){
			keepRunning = false;
		}

		// Selection system
		if(IsKeyPressed(KEY_TAB)){
			selected ++;
			if (selected > 9) {
				selected = 0;
			}
			InputManager(inputs, scene, selected);

			for(Entity e = 0; e < scene.entityMasks.size(); e++) {
				if(!scene.HasComponent<Rendering>(e)) continue;
				auto& rendering = scene.GetComponent<Rendering>(e);

				if(e != selected)
					rendering.drawBoundingBox = false;
				else rendering.drawBoundingBox = true;
			}
		}

		// Stuff
		inputs.PollEvents();
		auto& transform = scene.GetComponent<TransformComp>(selected);
		Update(scene);

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

				
				DrawSystem(scene);
				

			}
			camera.EndMode();

			// Measure our FPS
			DrawFPS(10, 10);
		}
		window.EndDrawing();
	}
	return 0;
}