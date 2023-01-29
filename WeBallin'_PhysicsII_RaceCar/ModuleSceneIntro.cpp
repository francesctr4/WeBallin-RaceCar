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

	InfernalMapLimitCreation();

	// Create Ramps

	InfernalRampsCreation();

	// Create Bumps

	for (float i = 0, j = 0; i < 6 * 10; i = i + 10, j = j + 1.75f) {

		bumps.emplace_back(CreateBump(0, 1 + j, 60 + i));

	}

	bumps.emplace_back(CreateBump(0, 6, 90));

	// Debug Variables

	enableGravity = true;

	enableLift = true;

	enableDrag = true;

	// Checkpoints

	checkpoint_1.emplace_back(CreateSensor(15,13,320,50,2,2,1));
	checkpoint_2.emplace_back(CreateSensor(-120, 13, 390, 2, 2, 50, 2));
	checkpoint_3.emplace_back(CreateSensor(-510, 13, 430, 2, 2, 50, 3));
	checkpoint_4.emplace_back(CreateSensor(-330, 13, 130, 2, 2, 50, 4));
	checkpoint_5.emplace_back(CreateSensor(-60, 13, 220, 50, 2, 2, 5));
	checkpoint_final.emplace_back(CreateSensor(0, 0, -3, 50, 2, 2, 6));

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

	for (auto& sensor : checkpoint_1) {

		sensor->Render();

	}

	for (auto& sensor : checkpoint_2) {

		sensor->Render();

	}

	for (auto& sensor : checkpoint_3) {

		sensor->Render();

	}

	for (auto& sensor : checkpoint_4) {

		sensor->Render();

	}

	for (auto& sensor : checkpoint_5) {

		sensor->Render();

	}

	for (auto& sensor : checkpoint_final) {

		sensor->Render();

	}

	
	// DEBUG KEYS -------------------------------------------------------------------------------------------------------

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) { // Toggle show colliders

		App->physics->debug = !App->physics->debug;

	}
		
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) { // Toggle camera control

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

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) { // Restart from checkpoint

		App->player->vehicle->vehicle->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		App->player->vehicle->vehicle->getRigidBody()->clearForces();
		App->player->vehicle->vehicle->getRigidBody()->setWorldTransform(App->player->checkpointPos);

	}

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN && App->scene_intro->checkpoint_final.empty()) { // Reset checkpoints when arrived at the end

		App->scene_intro->checkpoint_1.emplace_back(App->scene_intro->CreateSensor(15, 13, 320, 50, 2, 2, 1));
		App->scene_intro->checkpoint_2.emplace_back(App->scene_intro->CreateSensor(-120, 13, 390, 2, 2, 50, 2));
		App->scene_intro->checkpoint_3.emplace_back(App->scene_intro->CreateSensor(-510, 13, 430, 2, 2, 50, 3));
		App->scene_intro->checkpoint_4.emplace_back(App->scene_intro->CreateSensor(-330, 13, 130, 2, 2, 50, 4));
		App->scene_intro->checkpoint_5.emplace_back(App->scene_intro->CreateSensor(-60, 13, 220, 50, 2, 2, 5));
		App->scene_intro->checkpoint_final.emplace_back(App->scene_intro->CreateSensor(0, 0, -3, 50, 2, 2, 6));

	}

	return UPDATE_CONTINUE;
}

Cube* ModuleSceneIntro::CreateGround() {

	Cube* ground = new Cube(3000, 0, 3000);
	ground->color.Set(0, 38, 0);

	return ground;

}

Cube* ModuleSceneIntro::CreateMapLimit(int posX, float posY, float posZ) {

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

Cube* ModuleSceneIntro::CreateSensor(float posX, float posY, float posZ, float sizeX, float sizeY, float sizeZ, int num) {

	Cube* sensor = new Cube(sizeX, sizeY, sizeZ);
	sensor->SetPos(posX, posY, posZ);

	sensor->color.r = 0;
	sensor->color.g = 0;
	sensor->color.b = 255;

	PhysBody3D* body = App->physics->AddBody(*sensor, 0);
	body->SetAsSensor(true);
	
	switch (num) {

		case 1: body->SetType(PhysBody3D::Type::CHECKPOINT_1); break;
		case 2: body->SetType(PhysBody3D::Type::CHECKPOINT_2); break;
		case 3: body->SetType(PhysBody3D::Type::CHECKPOINT_3); break;
		case 4: body->SetType(PhysBody3D::Type::CHECKPOINT_4); break;
		case 5: body->SetType(PhysBody3D::Type::CHECKPOINT_5); break;
		case 6: 
			body->SetType(PhysBody3D::Type::CHECKPOINT_FINAL); 
			sensor->color.r = 255;
			sensor->color.g = 0;
			sensor->color.b = 0;

			break;

	}

	return sensor;
}

// DANGEROUS ZONE AHEAD: NO TRESPASSING

void ModuleSceneIntro::InfernalRampsCreation() {

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

}

void ModuleSceneIntro::InfernalMapLimitCreation() {

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

	mapLimits.emplace_back(CreateMapLimit(21, 14, 275));
	mapLimits.emplace_back(CreateMapLimit(22, 14, 278));
	mapLimits.emplace_back(CreateMapLimit(23, 14, 281));
	mapLimits.emplace_back(CreateMapLimit(24, 14, 284));
	mapLimits.emplace_back(CreateMapLimit(25, 14, 287));
	mapLimits.emplace_back(CreateMapLimit(26, 14, 290));
	mapLimits.emplace_back(CreateMapLimit(27, 14, 293));
	mapLimits.emplace_back(CreateMapLimit(28, 14, 296));
	mapLimits.emplace_back(CreateMapLimit(29, 14, 299));
	mapLimits.emplace_back(CreateMapLimit(30, 14, 302));
	mapLimits.emplace_back(CreateMapLimit(31, 14, 305));
	mapLimits.emplace_back(CreateMapLimit(32, 14, 308));
	mapLimits.emplace_back(CreateMapLimit(33, 14, 311));
	mapLimits.emplace_back(CreateMapLimit(34, 14, 314));
	mapLimits.emplace_back(CreateMapLimit(35, 14, 317));
	mapLimits.emplace_back(CreateMapLimit(36, 14, 320));
	mapLimits.emplace_back(CreateMapLimit(37, 14, 323));
	mapLimits.emplace_back(CreateMapLimit(38, 14, 326));
	mapLimits.emplace_back(CreateMapLimit(39, 14, 329));
	mapLimits.emplace_back(CreateMapLimit(40, 14, 332));
	mapLimits.emplace_back(CreateMapLimit(41, 14, 335));
	mapLimits.emplace_back(CreateMapLimit(42, 14, 338));
	mapLimits.emplace_back(CreateMapLimit(43, 14, 341));
	mapLimits.emplace_back(CreateMapLimit(44, 14, 344));
	mapLimits.emplace_back(CreateMapLimit(45, 14, 347));
	mapLimits.emplace_back(CreateMapLimit(46, 14, 350));
	mapLimits.emplace_back(CreateMapLimit(47, 14, 353));
	mapLimits.emplace_back(CreateMapLimit(48, 14, 356));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 359));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 362));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 365));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 368));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 371));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 374));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 377));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 380));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 383));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 386));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 389));
	mapLimits.emplace_back(CreateMapLimit(49, 14, 392));
	mapLimits.emplace_back(CreateMapLimit(46, 14, 392));
	mapLimits.emplace_back(CreateMapLimit(43, 14, 392));
	mapLimits.emplace_back(CreateMapLimit(40, 14, 392));
	mapLimits.emplace_back(CreateMapLimit(37, 14, 392));
	mapLimits.emplace_back(CreateMapLimit(34, 14, 393));
	mapLimits.emplace_back(CreateMapLimit(31, 14, 394));
	mapLimits.emplace_back(CreateMapLimit(28, 14, 395));
	mapLimits.emplace_back(CreateMapLimit(25, 14, 396));
	mapLimits.emplace_back(CreateMapLimit(22, 14, 397));
	mapLimits.emplace_back(CreateMapLimit(19, 14, 398));
	mapLimits.emplace_back(CreateMapLimit(16, 14, 399));
	mapLimits.emplace_back(CreateMapLimit(13, 14, 400));
	mapLimits.emplace_back(CreateMapLimit(10, 14, 401));
	mapLimits.emplace_back(CreateMapLimit(7, 14, 402));
	mapLimits.emplace_back(CreateMapLimit(4, 14, 403));
	mapLimits.emplace_back(CreateMapLimit(1, 14, 404));
	mapLimits.emplace_back(CreateMapLimit(-2, 14, 405));
	mapLimits.emplace_back(CreateMapLimit(-5, 14, 406));
	mapLimits.emplace_back(CreateMapLimit(-8, 14, 407));
	mapLimits.emplace_back(CreateMapLimit(-11, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-14, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-17, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-20, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-23, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-26, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-29, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-32, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-35, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-38, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-41, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-44, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-47, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-50, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-53, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-56, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-59, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-62, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-65, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-68, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-71, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-74, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-77, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-80, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-83, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-86, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-89, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-92, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-95, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-98, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-101, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-104, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-107, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-110, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-113, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-116, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-119, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-122, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-125, 14, 408));
	mapLimits.emplace_back(CreateMapLimit(-128, 14, 407));
	mapLimits.emplace_back(CreateMapLimit(-131, 14, 406));
	mapLimits.emplace_back(CreateMapLimit(-134, 14, 405));
	mapLimits.emplace_back(CreateMapLimit(-137, 14, 404));
	mapLimits.emplace_back(CreateMapLimit(-140, 14, 403));
	mapLimits.emplace_back(CreateMapLimit(-143, 14, 402));
	mapLimits.emplace_back(CreateMapLimit(-146, 14, 401));
	mapLimits.emplace_back(CreateMapLimit(-149, 14, 400));
	mapLimits.emplace_back(CreateMapLimit(-152, 14, 399));
	mapLimits.emplace_back(CreateMapLimit(-155, 14, 398));
	mapLimits.emplace_back(CreateMapLimit(-158, 14, 397));
	mapLimits.emplace_back(CreateMapLimit(-161, 14, 396));
	mapLimits.emplace_back(CreateMapLimit(-164, 14, 395));
	mapLimits.emplace_back(CreateMapLimit(-167, 14, 394));
	mapLimits.emplace_back(CreateMapLimit(-170, 14, 393));
	mapLimits.emplace_back(CreateMapLimit(-173, 14, 392));
	mapLimits.emplace_back(CreateMapLimit(-176, 14, 391));
	mapLimits.emplace_back(CreateMapLimit(-179, 14, 390));
	mapLimits.emplace_back(CreateMapLimit(-182, 14, 389));
	mapLimits.emplace_back(CreateMapLimit(-185, 14, 388));
	mapLimits.emplace_back(CreateMapLimit(-188, 14, 387));
	mapLimits.emplace_back(CreateMapLimit(-191, 14, 386));
	mapLimits.emplace_back(CreateMapLimit(-194, 14, 385));
	mapLimits.emplace_back(CreateMapLimit(-197, 14, 384));
	mapLimits.emplace_back(CreateMapLimit(-200, 14, 383));
	mapLimits.emplace_back(CreateMapLimit(-203, 14, 382));
	mapLimits.emplace_back(CreateMapLimit(-206, 14, 381));
	mapLimits.emplace_back(CreateMapLimit(-209, 14, 380));
	mapLimits.emplace_back(CreateMapLimit(-212, 14, 379));
	mapLimits.emplace_back(CreateMapLimit(-215, 14, 378));
	mapLimits.emplace_back(CreateMapLimit(-218, 14, 377));
	mapLimits.emplace_back(CreateMapLimit(-221, 14, 376));
	mapLimits.emplace_back(CreateMapLimit(-224, 14, 375));
	mapLimits.emplace_back(CreateMapLimit(-227, 14, 374));
	mapLimits.emplace_back(CreateMapLimit(-230, 14, 373));
	mapLimits.emplace_back(CreateMapLimit(-233, 14, 372));
	mapLimits.emplace_back(CreateMapLimit(-236, 14, 371));
	mapLimits.emplace_back(CreateMapLimit(-239, 14, 370));
	mapLimits.emplace_back(CreateMapLimit(-242, 14, 369));
	mapLimits.emplace_back(CreateMapLimit(-245, 14, 368));
	mapLimits.emplace_back(CreateMapLimit(-248, 14, 367));
	mapLimits.emplace_back(CreateMapLimit(-251, 14, 366));
	mapLimits.emplace_back(CreateMapLimit(-254, 14, 365));
	mapLimits.emplace_back(CreateMapLimit(-257, 14, 364));
	mapLimits.emplace_back(CreateMapLimit(-260, 14, 363));
	mapLimits.emplace_back(CreateMapLimit(-263, 14, 362));
	mapLimits.emplace_back(CreateMapLimit(-266, 14, 361));
	mapLimits.emplace_back(CreateMapLimit(-269, 14, 360));
	mapLimits.emplace_back(CreateMapLimit(-272, 14, 359));
	mapLimits.emplace_back(CreateMapLimit(-275, 14, 358));
	mapLimits.emplace_back(CreateMapLimit(-278, 14, 357));
	mapLimits.emplace_back(CreateMapLimit(-281, 14, 356));
	mapLimits.emplace_back(CreateMapLimit(-284, 14, 355));
	mapLimits.emplace_back(CreateMapLimit(-287, 14, 354));
	mapLimits.emplace_back(CreateMapLimit(-290, 14, 353));
	mapLimits.emplace_back(CreateMapLimit(-293, 14, 352));
	mapLimits.emplace_back(CreateMapLimit(-296, 14, 351));
	mapLimits.emplace_back(CreateMapLimit(-299, 14, 350));
	mapLimits.emplace_back(CreateMapLimit(-302, 14, 349));
	mapLimits.emplace_back(CreateMapLimit(-305, 14, 348));
	mapLimits.emplace_back(CreateMapLimit(-308, 14, 347));
	mapLimits.emplace_back(CreateMapLimit(-311, 14, 346));
	mapLimits.emplace_back(CreateMapLimit(-314, 14, 345));
	mapLimits.emplace_back(CreateMapLimit(-317, 14, 344));
	mapLimits.emplace_back(CreateMapLimit(-320, 14, 343));
	mapLimits.emplace_back(CreateMapLimit(-323, 14, 342));
	mapLimits.emplace_back(CreateMapLimit(-326, 14, 341));
	mapLimits.emplace_back(CreateMapLimit(-329, 14, 340));
	mapLimits.emplace_back(CreateMapLimit(-332, 14, 339));
	mapLimits.emplace_back(CreateMapLimit(-335, 14, 338));
	mapLimits.emplace_back(CreateMapLimit(-338, 14, 337));
	mapLimits.emplace_back(CreateMapLimit(-341, 14, 336));
	mapLimits.emplace_back(CreateMapLimit(-344, 14, 335));
	mapLimits.emplace_back(CreateMapLimit(-347, 14, 334));
	mapLimits.emplace_back(CreateMapLimit(-350, 14, 333));
	mapLimits.emplace_back(CreateMapLimit(-353, 14, 332));
	mapLimits.emplace_back(CreateMapLimit(-356, 14, 331));
	mapLimits.emplace_back(CreateMapLimit(-359, 14, 330));
	mapLimits.emplace_back(CreateMapLimit(-362, 14, 329));
	mapLimits.emplace_back(CreateMapLimit(-365, 14, 328));
	mapLimits.emplace_back(CreateMapLimit(-368, 14, 327));
	mapLimits.emplace_back(CreateMapLimit(-371, 14, 326));
	mapLimits.emplace_back(CreateMapLimit(-374, 14, 325));
	mapLimits.emplace_back(CreateMapLimit(-377, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-380, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-383, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-386, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-389, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-392, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-395, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-398, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-401, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-404, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-407, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-410, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-413, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-416, 14, 324));
	mapLimits.emplace_back(CreateMapLimit(-419, 14, 326));
	mapLimits.emplace_back(CreateMapLimit(-422, 14, 328));
	mapLimits.emplace_back(CreateMapLimit(-425, 14, 330));
	mapLimits.emplace_back(CreateMapLimit(-428, 14, 332));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 334));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 337));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 340));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 343));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 346));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 349));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 352));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 355));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 358));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 361));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 364));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 367));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 370));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 373));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 376));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 379));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 382));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 385));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 388));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 391));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 394));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 397));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 400));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 403));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 406));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 409));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 412));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 415));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 418));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 421));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 424));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 427));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 430));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 433));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 436));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 439));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 442));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 445));
	mapLimits.emplace_back(CreateMapLimit(-430, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-433, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-436, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-439, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-442, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-445, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-448, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-451, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-454, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-457, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-460, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-463, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-466, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-469, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-472, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-475, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-478, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-481, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-484, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-487, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-490, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-493, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-496, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-499, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-502, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-505, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-508, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-511, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-514, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-517, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-520, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-523, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-526, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-529, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-532, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-535, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-538, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-541, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-544, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-547, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-550, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-553, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-556, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-559, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-562, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-565, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-568, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-571, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-574, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-577, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-580, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-583, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-586, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 448));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 445));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 442));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 439));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 436));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 433));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 430));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 427));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 424));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 421));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 418));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 415));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 412));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 409));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 406));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 403));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 400));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 397));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 394));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 391));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 388));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 385));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 382));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 379));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 376));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 373));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 370));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 367));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 364));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 361));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 358));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 355));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 352));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 349));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 346));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 343));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 340));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 337));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 334));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 331));
	mapLimits.emplace_back(CreateMapLimit(-589, 14, 328));
	mapLimits.emplace_back(CreateMapLimit(-589, 15, 325));
	mapLimits.emplace_back(CreateMapLimit(-589, 15, 322));
	mapLimits.emplace_back(CreateMapLimit(-589, 15.5f, 319));
	mapLimits.emplace_back(CreateMapLimit(-589, 16, 316));
	mapLimits.emplace_back(CreateMapLimit(-589, 16.5f, 313));
	mapLimits.emplace_back(CreateMapLimit(-589, 17, 310));
	mapLimits.emplace_back(CreateMapLimit(-589, 17.5f, 307));
	mapLimits.emplace_back(CreateMapLimit(-589, 18, 304));
	mapLimits.emplace_back(CreateMapLimit(-589, 18.5f, 301));
	mapLimits.emplace_back(CreateMapLimit(-589, 19, 298));
	mapLimits.emplace_back(CreateMapLimit(-589, 19.5f, 295));
	mapLimits.emplace_back(CreateMapLimit(-589, 20, 292));
	mapLimits.emplace_back(CreateMapLimit(-589, 20.5f, 289));
	mapLimits.emplace_back(CreateMapLimit(-589, 21, 286));
	mapLimits.emplace_back(CreateMapLimit(-589, 21.5f, 283));
	mapLimits.emplace_back(CreateMapLimit(-589, 22, 280));
	mapLimits.emplace_back(CreateMapLimit(-589, 22.5f, 277));
	mapLimits.emplace_back(CreateMapLimit(-589, 23, 274));
	mapLimits.emplace_back(CreateMapLimit(-589, 23.5f, 271));
	mapLimits.emplace_back(CreateMapLimit(-589, 24, 268));
	mapLimits.emplace_back(CreateMapLimit(-589, 24.5f, 265));
	mapLimits.emplace_back(CreateMapLimit(-589, 25, 262));
	mapLimits.emplace_back(CreateMapLimit(-589, 25.5f, 259));
	mapLimits.emplace_back(CreateMapLimit(-589, 26, 256));
	mapLimits.emplace_back(CreateMapLimit(-589, 26.5f, 253));
	mapLimits.emplace_back(CreateMapLimit(-589, 27, 250));
	mapLimits.emplace_back(CreateMapLimit(-589, 27.5f, 247));
	mapLimits.emplace_back(CreateMapLimit(-589, 28, 244));
	mapLimits.emplace_back(CreateMapLimit(-589, 28.5f, 241));
	mapLimits.emplace_back(CreateMapLimit(-589, 29, 238));
	mapLimits.emplace_back(CreateMapLimit(-589, 29.5f, 235));
	mapLimits.emplace_back(CreateMapLimit(-589, 30, 232));
	mapLimits.emplace_back(CreateMapLimit(-589, 30.5f, 229));
	mapLimits.emplace_back(CreateMapLimit(-589, 31, 226));
	mapLimits.emplace_back(CreateMapLimit(-589, 31.5f, 223));
	mapLimits.emplace_back(CreateMapLimit(-589, 32, 220));
	mapLimits.emplace_back(CreateMapLimit(-589, 32.5f, 217));
	mapLimits.emplace_back(CreateMapLimit(-589, 33, 214));
	mapLimits.emplace_back(CreateMapLimit(-589, 33.5f, 211));
	mapLimits.emplace_back(CreateMapLimit(-589, 34, 208));
	mapLimits.emplace_back(CreateMapLimit(-589, 34.5f, 205));
	mapLimits.emplace_back(CreateMapLimit(-589, 35, 202));
	mapLimits.emplace_back(CreateMapLimit(-589, 35.5f, 199));
	mapLimits.emplace_back(CreateMapLimit(-589, 36, 196));
	mapLimits.emplace_back(CreateMapLimit(-589, 36.5f, 193));
	mapLimits.emplace_back(CreateMapLimit(-589, 37, 190));
	mapLimits.emplace_back(CreateMapLimit(-589, 37.5f, 187));
	mapLimits.emplace_back(CreateMapLimit(-589, 38, 184));
	mapLimits.emplace_back(CreateMapLimit(-589, 38.5f, 181));
	mapLimits.emplace_back(CreateMapLimit(-589, 39, 178));
	mapLimits.emplace_back(CreateMapLimit(-589, 39.5f, 175));
	mapLimits.emplace_back(CreateMapLimit(-589, 40, 172));
	mapLimits.emplace_back(CreateMapLimit(-589, 40.5f, 169));
	mapLimits.emplace_back(CreateMapLimit(-589, 41, 166));
	mapLimits.emplace_back(CreateMapLimit(-589, 41.5f, 163));
	mapLimits.emplace_back(CreateMapLimit(-589, 42, 160));
	mapLimits.emplace_back(CreateMapLimit(-589, 42.5f, 157));
	mapLimits.emplace_back(CreateMapLimit(-589, 43, 154));
	mapLimits.emplace_back(CreateMapLimit(-589, 43.5f, 151));
	mapLimits.emplace_back(CreateMapLimit(-589, 44, 148));
	mapLimits.emplace_back(CreateMapLimit(-589, 44.5f, 145));
	mapLimits.emplace_back(CreateMapLimit(-589, 45, 142));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 139));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 136));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 133));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 130));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 127));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 124));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 121));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 118));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 115));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 112));
	mapLimits.emplace_back(CreateMapLimit(-589, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-586, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-583, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-580, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-577, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-574, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-571, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-568, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-565, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-562, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-559, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-556, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-553, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-550, 45.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-547, 45, 110));
	mapLimits.emplace_back(CreateMapLimit(-544, 44, 110));
	mapLimits.emplace_back(CreateMapLimit(-541, 43.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-538, 43, 110));
	mapLimits.emplace_back(CreateMapLimit(-535, 42.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-532, 42, 110));
	mapLimits.emplace_back(CreateMapLimit(-529, 41.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-526, 41, 110));
	mapLimits.emplace_back(CreateMapLimit(-523, 40.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-520, 40, 110));
	mapLimits.emplace_back(CreateMapLimit(-517, 39.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-514, 39, 110));
	mapLimits.emplace_back(CreateMapLimit(-511, 38.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-508, 38, 110));
	mapLimits.emplace_back(CreateMapLimit(-505, 37.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-502, 37, 110));
	mapLimits.emplace_back(CreateMapLimit(-499, 36.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-496, 36, 110));
	mapLimits.emplace_back(CreateMapLimit(-493, 35.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-490, 35, 110));
	mapLimits.emplace_back(CreateMapLimit(-487, 34.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-484, 34, 110));
	mapLimits.emplace_back(CreateMapLimit(-481, 33.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-478, 33, 110));
	mapLimits.emplace_back(CreateMapLimit(-475, 32.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-472, 32, 110));
	mapLimits.emplace_back(CreateMapLimit(-469, 31.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-466, 31, 110));
	mapLimits.emplace_back(CreateMapLimit(-463, 30.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-460, 30, 110));
	mapLimits.emplace_back(CreateMapLimit(-457, 29.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-454, 29, 110));
	mapLimits.emplace_back(CreateMapLimit(-451, 28.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-448, 28, 110));
	mapLimits.emplace_back(CreateMapLimit(-445, 27.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-442, 27, 110));
	mapLimits.emplace_back(CreateMapLimit(-439, 26.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-436, 26, 110));
	mapLimits.emplace_back(CreateMapLimit(-433, 25.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-430, 25, 110));
	mapLimits.emplace_back(CreateMapLimit(-427, 24.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-424, 24, 110));
	mapLimits.emplace_back(CreateMapLimit(-421, 23.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-418, 23, 110));
	mapLimits.emplace_back(CreateMapLimit(-415, 22.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-412, 22, 110));
	mapLimits.emplace_back(CreateMapLimit(-409, 21.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-406, 21, 110));
	mapLimits.emplace_back(CreateMapLimit(-403, 20.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-400, 20, 110));
	mapLimits.emplace_back(CreateMapLimit(-397, 19.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-394, 19, 110));
	mapLimits.emplace_back(CreateMapLimit(-391, 18.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-388, 18, 110));
	mapLimits.emplace_back(CreateMapLimit(-385, 17.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-382, 17, 110));
	mapLimits.emplace_back(CreateMapLimit(-379, 16.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-376, 16, 110));
	mapLimits.emplace_back(CreateMapLimit(-373, 15.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-370, 15, 110));
	mapLimits.emplace_back(CreateMapLimit(-367, 14.5f, 110));
	mapLimits.emplace_back(CreateMapLimit(-364, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-361, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-358, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-355, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-352, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-349, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-346, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-343, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-340, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-337, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-334, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-331, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-328, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-325, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-322, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-319, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-316, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-313, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-310, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-307, 14, 110));
	mapLimits.emplace_back(CreateMapLimit(-304, 14, 111.5f));
	mapLimits.emplace_back(CreateMapLimit(-301, 14, 113));
	mapLimits.emplace_back(CreateMapLimit(-298, 14, 114.5f));
	mapLimits.emplace_back(CreateMapLimit(-295, 14, 116));
	mapLimits.emplace_back(CreateMapLimit(-292, 14, 117.5f));
	mapLimits.emplace_back(CreateMapLimit(-289, 14, 119));
	mapLimits.emplace_back(CreateMapLimit(-286, 14, 120.5f));
	mapLimits.emplace_back(CreateMapLimit(-283, 14, 122));
	mapLimits.emplace_back(CreateMapLimit(-280, 14, 123.5f));
	mapLimits.emplace_back(CreateMapLimit(-277, 14, 125));
	mapLimits.emplace_back(CreateMapLimit(-274, 14, 126.5f));
	mapLimits.emplace_back(CreateMapLimit(-271, 14, 128));
	mapLimits.emplace_back(CreateMapLimit(-268, 14, 129.5f));
	mapLimits.emplace_back(CreateMapLimit(-265, 14, 131));
	mapLimits.emplace_back(CreateMapLimit(-262, 14, 132.5f));
	mapLimits.emplace_back(CreateMapLimit(-259, 14, 134));
	mapLimits.emplace_back(CreateMapLimit(-256, 14, 135.5f));
	mapLimits.emplace_back(CreateMapLimit(-253, 14, 137));
	mapLimits.emplace_back(CreateMapLimit(-250, 14, 138.5f));
	mapLimits.emplace_back(CreateMapLimit(-247, 14, 140));
	mapLimits.emplace_back(CreateMapLimit(-244, 14, 141.5f));
	mapLimits.emplace_back(CreateMapLimit(-241, 14, 143));
	mapLimits.emplace_back(CreateMapLimit(-238, 14, 144.5f));
	mapLimits.emplace_back(CreateMapLimit(-235, 14, 146));
	mapLimits.emplace_back(CreateMapLimit(-232, 14, 147.5f));
	mapLimits.emplace_back(CreateMapLimit(-229, 14, 149));
	mapLimits.emplace_back(CreateMapLimit(-226, 14, 150.5f));
	mapLimits.emplace_back(CreateMapLimit(-223, 14, 152));
	mapLimits.emplace_back(CreateMapLimit(-220, 14, 153.5f));
	mapLimits.emplace_back(CreateMapLimit(-217, 14, 155));
	mapLimits.emplace_back(CreateMapLimit(-214, 14, 156.5f));
	mapLimits.emplace_back(CreateMapLimit(-211, 14, 158));
	mapLimits.emplace_back(CreateMapLimit(-208, 14, 159.5f));
	mapLimits.emplace_back(CreateMapLimit(-205, 14, 161));
	mapLimits.emplace_back(CreateMapLimit(-202, 14, 162.5f));
	mapLimits.emplace_back(CreateMapLimit(-199, 14, 164));
	mapLimits.emplace_back(CreateMapLimit(-196, 14, 165.5f));
	mapLimits.emplace_back(CreateMapLimit(-193, 14, 167));
	mapLimits.emplace_back(CreateMapLimit(-190, 14, 168.5f));
	mapLimits.emplace_back(CreateMapLimit(-187, 14, 170));
	mapLimits.emplace_back(CreateMapLimit(-184, 14, 171.5f));
	mapLimits.emplace_back(CreateMapLimit(-181, 14, 173));
	mapLimits.emplace_back(CreateMapLimit(-178, 14, 174.5f));
	mapLimits.emplace_back(CreateMapLimit(-175, 14, 176));
	mapLimits.emplace_back(CreateMapLimit(-172, 14, 177.5f));
	mapLimits.emplace_back(CreateMapLimit(-169, 14, 179));
	mapLimits.emplace_back(CreateMapLimit(-166, 14, 180.5f));
	mapLimits.emplace_back(CreateMapLimit(-163, 14, 182));
	mapLimits.emplace_back(CreateMapLimit(-160, 14, 183.5f));
	mapLimits.emplace_back(CreateMapLimit(-157, 14, 185));
	mapLimits.emplace_back(CreateMapLimit(-154, 14, 186.5f));
	mapLimits.emplace_back(CreateMapLimit(-151, 14, 188));
	mapLimits.emplace_back(CreateMapLimit(-148, 14, 189.5f));
	mapLimits.emplace_back(CreateMapLimit(-145, 14, 191));
	mapLimits.emplace_back(CreateMapLimit(-142, 14, 192.5f));
	mapLimits.emplace_back(CreateMapLimit(-139, 14, 194));
	mapLimits.emplace_back(CreateMapLimit(-136, 14, 195.5f));
	mapLimits.emplace_back(CreateMapLimit(-133, 14, 197));

	for (int i = 0; i < 17 * 3; i = i + 3) { //mitad de curva prerampa

		mapLimits.emplace_back(CreateMapLimit(-82 - i, 14, 270));

	}

	for (int i = 0; i < 25 * 3; i = i + 3) { //prerrampa final

		mapLimits.emplace_back(CreateMapLimit(-132, 14, 270 - i));

	}

	for (int i = 0; i < 48 * 3; i = i + 3) { //prerrampa final

		mapLimits.emplace_back(CreateMapLimit(-82, 14, 20 + 13 * 3 + 23 * 3 + i));

	}

	for (float i = 0, j = 0; i < 23 * 3; i = i + 3, j = j + 0.6f) { //rampa final

		mapLimits.emplace_back(CreateMapLimit(-42, 1 + j, 20 + 13 * 3 + i));
		mapLimits.emplace_back(CreateMapLimit(-82, 1 + j, 20 + 13 * 3 + i));

	}

	for (int i = 0; i < 29 * 3; i = i + 3) { //post rampa final

		mapLimits.emplace_back(CreateMapLimit(-42, 1, 20 + 13 * 3 - i));
		mapLimits.emplace_back(CreateMapLimit(-82, 1, 20 + 13 * 3 - i));

	}

	for (int i = 0; i < 12 * 3; i = i + 3) { //curva final 1

		mapLimits.emplace_back(CreateMapLimit(20, 1, -28 - i));
		mapLimits.emplace_back(CreateMapLimit(-82, 1, -28 - i));

	}

	for (int i = 0; i < 14 * 3; i = i + 3) { //exterior curva final

		mapLimits.emplace_back(CreateMapLimit(-42 - i, 1, -65));

	}

	mapLimits.emplace_back(CreateMapLimit(20, 1, -25));

	for (int i = 0; i < 8 * 3; i = i + 3) { //mitad de curva final

		mapLimits.emplace_back(CreateMapLimit(-20 - i, 1, -25));
		mapLimits.emplace_back(CreateMapLimit(-20 - i, 1, -65));

	}

	for (int i = 0; i < 14 * 3; i = i + 3) { //exterior curva final

		mapLimits.emplace_back(CreateMapLimit(-20 + i, 1, -65));

	}


	for (int i = 0; i < 15 * 3; i = i + 3) { //prerrampa

		mapLimits.emplace_back(CreateMapLimit(20, 1, 20 - i));
		mapLimits.emplace_back(CreateMapLimit(-20, 1, 20 - i));

	}

}