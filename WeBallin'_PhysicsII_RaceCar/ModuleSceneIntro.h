#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include <vector>

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:

	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;

	bool cameraControl;

	Cube* CreateGround();
	Cube* ground;

	Cube* CreateMapLimit(int posX, int posY, int posZ);
	std::vector<Cube*> mapLimits;

	Cube* CreateRamp(int posX, float posY, int posZ, int sizeX, int sizeY, int sizeZ, int angleRotX, int angleRotY, int angleRotZ);
	std::vector<Cube*> ramps;

	Cylinder* CreateBump(float posX, float posY, float posZ);
	std::vector<Cylinder*> bumps;

	Cube* CreateMapLimit(int posX, int posY, int posZ);
	std::vector<Cube*> sensors;

	bool enableGravity;
	bool enableLift;
	bool enableDrag;

};
