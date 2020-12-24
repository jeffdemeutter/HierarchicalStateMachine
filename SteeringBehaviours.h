#pragma once

#include "IExamInterface.h"

using TargetData = Elite::Vector2;

class SteeringBehavior
{
public:
	SteeringBehavior() = default;
	virtual ~SteeringBehavior() = default;

	virtual SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

protected:
	TargetData m_Target;
};

///////////////////////////////////////
//SEEK
//****
class Seek : public SteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	virtual SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;
};

//////////////////////////
//WANDER
//******
class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behavior
	virtual SteeringPlugin_Output CalculateSteering(float deltaT, AgentInfo pAgent) override;

	void SetWanderOffset(float offset) { m_Offset = offset; };
	void SetWanderRadius(float radius) { m_Radius = radius; };
	void SetMaxAngleChange(float rad) { m_AngleChange = rad; };

protected:
	float m_Offset = 6.f;
	float m_Radius = 4.f;
	float m_AngleChange = Elite::ToRadians(45);
	float m_WanderAngle = 0.f;
};