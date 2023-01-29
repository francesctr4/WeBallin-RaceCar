#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->audio->PlayMusic("Audio/GameTheme.ogg");

	App->camera->LookAt(vec3(0, 0, 0));

	cameraControl = false;

	// Create Ground

	ground = CreateGround();

	// Create Map Limits

	for (int i = 0; i < 13 * 3; i = i + 3) { //prerrampa

		mapLimits.emplace_back(CreateMapLimit(20, 1, 20 + i));
		mapLimits.emplace_back(CreateMapLimit(-20, 1, 20 + i));

	}

	for (float i = 0, j = 0; i < 23 * 3; i = i + 3, j = j + 0.6f) { //en rampa

		mapLimits.emplace_back(CreateMapLimit(20, 1 + j, 20 + 13 * 3 + i));
		mapLimits.emplace_back(CreateMapLimit(-20, 1 + j, 20 + 13 * 3 + i));

	}

	for (int i = 0; i < 49 * 3; i = i + 3) { //postrrampa

		mapLimits.emplace_back(CreateMapLimit(20, 14, 20 + 13 * 3 + 23 * 3 + i));
		mapLimits.emplace_back(CreateMapLimit(-20, 14, 20 + 13 * 3 + 23 * 3 + i));

	}

	mapLimits.emplace_back(CreateMapLimit(20, 14, 20 + 13 * 3 + 23 * 3));

	// Create Ramps

	ramps.emplace_back(CreateRamp(0, -2, 50, 40, 150, 2, 80, 0, 0));
	ramps.emplace_back(CreateRamp(0, 11, 199, 40, 150, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(10, 11, 294, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(20, 11, 334, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(30, 11, 374, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-10, 11, 388, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-50, 11, 388, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-90, 11, 388, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-130, 11, 388, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-170, 11, 374, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-210, 11, 360, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-250, 11, 346, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-290, 11, 332, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-330, 11, 318, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-370, 11, 304, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-410, 11, 304, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-450, 11, 318, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-450, 11, 348, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-450, 11, 388, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-450, 11, 428, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-490, 11, 428, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-530, 11, 428, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-570, 11, 428, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-570, 11, 388, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-570, 11, 348, 40, 40, 2, 90, 0, 0));

	ramps.emplace_back(CreateRamp(-560, 11, 308, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-560, 11, 268, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-560, 11, 228, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-560, 11, 188, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-560, 11, 168, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-560, 11, 130, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-540, 11, 130, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-510, 11, 140, 40, 20, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-470, 11, 140, 40, 20, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-431, 14.4f, 140, 40, 2, 20, 0, 0, 10));

	ramps.emplace_back(CreateRamp(-580, 11, 328, 20, 40, 2, 100, 0, 0));
	ramps.emplace_back(CreateRamp(-580, 14.5f, 308, 20, 40, 2, 100, 0, 0));
	ramps.emplace_back(CreateRamp(-580, 18, 288, 20, 40, 2, 100, 0, 0));
	ramps.emplace_back(CreateRamp(-580, 21.5f, 268, 20, 40, 2, 100, 0, 0));
	ramps.emplace_back(CreateRamp(-580, 25, 248, 20, 40, 2, 100, 0, 0));
	ramps.emplace_back(CreateRamp(-580, 28.5f, 228, 20, 40, 2, 100, 0, 0));
	ramps.emplace_back(CreateRamp(-580, 32, 208, 20, 40, 2, 100, 0, 0));
	ramps.emplace_back(CreateRamp(-580, 35.5f, 188, 20, 40, 2, 100, 0, 0));
	ramps.emplace_back(CreateRamp(-580, 39, 168, 20, 40, 2, 100, 0, 0));
	ramps.emplace_back(CreateRamp(-580, 42.5f, 130, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-560, 42.5f, 130, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-532, 39, 120, 40, 2, 20, 0, 0, -10));
	ramps.emplace_back(CreateRamp(-502, 33.7f, 120, 40, 2, 20, 0, 0, -10));
	ramps.emplace_back(CreateRamp(-482, 30.2f, 120, 40, 2, 20, 0, 0, -10));
	ramps.emplace_back(CreateRamp(-462, 26.7f, 120, 40, 2, 20, 0, 0, -10));
	ramps.emplace_back(CreateRamp(-442, 23.2f, 120, 40, 2, 20, 0, 0, -10));
	ramps.emplace_back(CreateRamp(-422, 19.7f, 120, 40, 2, 20, 0, 0, -10));
	ramps.emplace_back(CreateRamp(-402, 16.2f, 120, 40, 2, 20, 0, 0, -10));
	ramps.emplace_back(CreateRamp(-382, 12.7f, 120, 40, 2, 20, 0, 0, -10));

	ramps.emplace_back(CreateRamp(-352, 11, 130, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-312, 11, 130, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-272, 11, 150, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-232, 11, 170, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-192, 11, 190, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-152, 11, 210, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-152, 11, 250, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-152, 11, 290, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-112, 11, 290, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-72, 11, 290, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-52, 11, 290, 20, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-62, 11, 250, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-62, 11, 210, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-62, 11, 170, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-62, 11, 150, 40, 40, 2, 90, 0, 0));
	ramps.emplace_back(CreateRamp(-62, 7.5f, 111, 40, 40, 2, 80, 0, 0));
	ramps.emplace_back(CreateRamp(-62, 0.8f, 73, 40, 40, 2, 80, 0, 0));

	// Create Bumps

	for (float i = 0, j = 0; i < 6 * 10; i = i + 10, j = j + 1.75f) {

		bumps.emplace_back(CreateBump(0, 1 + j, 60 + i));

	}

	bumps.emplace_back(CreateBump(0, 6, 90));

	enableGravity = true;

	enableLift = true;

	enableDrag = true;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	// CAMERA -------------------------------------------------------------------------------------------------------------

	// Get some short-hand vars (for code clarity)
	auto veh = App->player->vehicle->vehicle;
	const auto& org = veh->getChassisWorldTransform().getOrigin(); // WORLD AXIS
	auto fwd = veh->getForwardVector().normalize(); // CHASSIS AXIS Z
	auto vri = veh->getRightVector().normalize(); // CHASSIS AXIS X
	auto vup = veh->getUpVector().normalize(); // CHASSIS AXIS Y
	
	if (!cameraControl) {

		// Set camera's reference point: same position as vehicle's center of mass
		App->camera->Reference = { org.x(), org.y(), org.z() };

		// Set camera's orientation: same orientation as vehicle (with X & Z axis reversed, due to camera's axis definition)
		App->camera->Z = { -fwd.getX(), -fwd.getY(), -fwd.getZ() }; // Vehicle forward axis (camera: Z = backwards axis) // CAMERA AXIS Z
		App->camera->X = { -vri.getX(), -vri.getY(), -vri.getZ() }; // Vehicle right axis (camera: X = left axis) // CAMERA AXIS X
		App->camera->Y = { +vup.getX(), +vup.getY(), +vup.getZ() }; // Vehicle up axis (camera: Y = up axis) // CAMERA AXIS Y

		/* Set camera's position: same position as vehicle's center of mass,
		   plus some arbitrary offsets >> IN FWD AND UP x,y,z DIRECTIONS << (!!!) */
		App->camera->Position = App->camera->Reference - 15 * vec3(fwd) + 4 * vec3(vup);

	}

	// SCENE ELEMENTS -------------------------------------------------------------------------------------------------------

	ground->Render();

	for (auto& mapLimit : mapLimits) {

		mapLimit->Render();

	}

	for (auto& ramp : ramps) {

		ramp->Render();

	}

	for (auto& bump : bumps) {

		bump->Render();

	}

	// DEBUG KEYS -------------------------------------------------------------------------------------------------------

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) { // Show colliders

		App->physics->debug = !App->physics->debug;

	}
		
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) { // Camera control

		cameraControl = !cameraControl;

	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) { // Restart the level
		
		App->player->vehicle->vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		App->player->vehicle->vehicle->getRigidBody()->clearForces();
		App->player->vehicle->vehicle->getRigidBody()->setWorldTransform(App->player->initialPos);
		
	}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) enableGravity = !enableGravity;  // Enable/Disable Gravity
	if (enableGravity) App->physics->world->setGravity(btVector3(0.0f, -9.807f, 0.0f));
	else App->physics->world->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) enableLift = !enableLift;  // Enable/Disable Lift
	if (enableLift) App->player->vehicle->Push(-5, 0, 0); // Lift FUYM

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) enableDrag = !enableDrag;  // Enable/Disable Drag
	if (enableDrag) {

		if (App->player->vehicle->GetKmh() >= 0) App->player->vehicle->Push(0, 0, -1); // -Drag FUYM
		if (App->player->vehicle->GetKmh() < 0) App->player->vehicle->Push(0, 0, 1); // +Drag FUYM

	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

Cube* ModuleSceneIntro::CreateGround() {

	Cube* ground = new Cube(3000, 0, 3000);
	ground->color.Set(0, 38, 0);

	return ground;

}

Cube* ModuleSceneIntro::CreateMapLimit(int posX, int posY, int posZ) {

	Cube* mapLimit = new Cube(1, 5, 1);
	mapLimit->SetPos(posX, posY, posZ);
	PhysBody3D* body = App->physics->AddBody(*mapLimit, 0);

	return mapLimit;
}

Cube* ModuleSceneIntro::CreateRamp(int posX, float posY, int posZ, int sizeX, int sizeY, int sizeZ, int angleRotX, int angleRotY, int angleRotZ) {

	Cube* ramp = new Cube(sizeX, sizeY, sizeZ);

	ramp->SetPos(posX, posY, posZ);

	if (angleRotX != 0)
	{
		ramp->SetRotation(angleRotX, vec3(1.0f, 0.0f, 0.0f));
	}
	
	if (angleRotY != 0)
	{
		ramp->SetRotation(angleRotY, vec3(0.0f, 1.0f, 0.0f));
	}

	if (angleRotZ != 0)
	{
		ramp->SetRotation(angleRotZ, vec3(0.0f, 0.0f, 1.0f));
	}

	PhysBody3D* body = App->physics->AddBody(*ramp, 0);

	return ramp;
}

Cylinder* ModuleSceneIntro::CreateBump(float posX, float posY, float posZ) {

	Cylinder* bump = new Cylinder(1, 38);

	bump->SetPos(posX, posY, posZ);

	PhysBody3D* body = App->physics->AddBody(*bump,0);

	return bump;
}

