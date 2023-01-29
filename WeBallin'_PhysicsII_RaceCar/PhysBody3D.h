#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"

class btRigidBody;
class Module;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;

	enum class Type { 

		PLAYER,
		CHECKPOINT_1, 
		CHECKPOINT_2,
		CHECKPOINT_3,
		CHECKPOINT_4,
		CHECKPOINT_5,
		CHECKPOINT_FINAL

	};

public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);

	void SetAsSensor(bool is_sensor);
	Type GetType();
	void SetType(Type type);

	void SetActive(bool active);
	

private:

	btRigidBody* body = nullptr;
	bool is_sensor = false;
	Type type;
	bool active = true;

public:

	p2List<Module*> collision_listeners;
	bool on_collision = true;

};

#endif // __PhysBody3D_H__