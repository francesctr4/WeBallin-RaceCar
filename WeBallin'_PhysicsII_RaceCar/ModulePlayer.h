#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1000.0f
#define MAX_DEACCELERATION -500.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 300.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:

	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;

	btTransform initialPos;
	btTransform checkpointPos;

	uint winFx;
	uint checkpointFx;

};