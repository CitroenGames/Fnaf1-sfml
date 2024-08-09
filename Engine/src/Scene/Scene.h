#pragma once
#include "ECS.h"

class Scene
{
public:
	virtual void Init() = 0;
	virtual void Update(double deltaTime);
	virtual void FixedUpdate();
	// this isnt needed if you are using layermanager but its here just in case
	virtual void Render() = 0;
	virtual void Destroy();

	std::shared_ptr<ECS::Entity> CreateEntity()
	{
		return world.CreateEntity();
	}

protected:
	ECS::World world;
};