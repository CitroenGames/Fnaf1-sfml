#pragma once
#include "Composable.h"

class Scene
{
public:
	virtual void Init() = 0;
	virtual void Update(double deltaTime);
	virtual void FixedUpdate();
	// this isnt needed if you are using layermanager but its here just in case
	virtual void Render() = 0;
	virtual void Destroy();

	std::shared_ptr<Composable::Node> CreateEntity()
	{
		return m_World.CreateNode();
	}

private:
	Composable::Scene m_World;
};