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

	App->camera->LookAt(vec3(0, 0, 0));

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
	// Camara

	vec3 cameraTarget = { App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().x() , App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().y(), App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().z() };
	//vec3 cameraPosition = { App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().x() , App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().y() + 4, App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().z() - 15 };

	vec3 cameraPosition = cameraTarget;

	//cameraPosition = rotate(cameraTarget.z - cameraPosition.z, App->player->vehicle->vehicle->getChassisWorldTransform().getRotation().getAngle(), vec3(0.0f, 1.0f, 0.0f));

	//cameraPosition = rotate(App->camera->Z, App->player->vehicle->vehicle->getChassisWorldTransform().getRotation().getAngle(), vec3(0.0f, 1.0f, 0.0f));

	//cameraPosition = rotate(cameraPosition - cameraTarget, App->player->vehicle->vehicle->getForwardVector().normalize().angle(btVector3(0.0f,0.0f, 1.0f)), cameraTarget.y);

	//cameraPosition = translate(App->player->vehicle->vehicle->getChassisWorldTransform().getRotation().getAxis().x(), App->player->vehicle->vehicle->getChassisWorldTransform().getRotation().getAxis().y(), App->player->vehicle->vehicle->getChassisWorldTransform().getRotation().getAxis().z()).translation();

	//App->camera->Position = { App->player->vehicle->vehicle->getForwardVector()., cameraTarget.y, App->player->vehicle->vehicle->getForwardVector().getZ()};

	App->camera->Z = { -App->player->vehicle->vehicle->getForwardVector().normalize().getX(), -App->player->vehicle->vehicle->getForwardVector().normalize().getY(), -App->player->vehicle->vehicle->getForwardVector().normalize().getZ() };
	//App->camera->Position = rotate(App->camera->Z, 20, cameraTarget.y);
	//App->camera->Z = normalize((App->camera->Position - App->camera->Reference));
	//App->camera->Z = normalize(rotate((App->camera->Position - App->camera->Reference), angle, vec3(0.0f, 1.0f, 0.0f)));
	//App->camera->X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), App->camera->Z));
	//App->camera->Y = cross(App->camera->Z, App->camera->X);

	App->camera->X = { -App->player->vehicle->vehicle->getRightVector().normalize().getX(), -App->player->vehicle->vehicle->getRightVector().normalize().getY(), -App->player->vehicle->vehicle->getRightVector().normalize().getZ() };

	App->camera->Y = { App->player->vehicle->vehicle->getUpVector().normalize().getX(), App->player->vehicle->vehicle->getUpVector().normalize().getY(), App->player->vehicle->vehicle->getUpVector().normalize().getZ() };

	App->camera->Reference = cameraTarget;
	
	float transform[16];
	App->player->vehicle->GetTransform(transform);

	App->camera->Position = { App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().x(), App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().y() + 4, App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().z() - 15 };
	//App->camera->Position = rotate(App->camera->Position, 1, cameraTarget);
	//App->camera->Reference = cameraTarget;
	//App->camera->LookAt(cameraTarget);

	// Suelo

	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

