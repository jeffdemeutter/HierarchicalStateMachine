/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "IExamInterface.h"

using TargetData = Elite::Vector2;

class SteeringBehavior
{
public:
	SteeringBehavior() = default;
	virtual ~SteeringBehavior() = default;

	virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agent) = 0;

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
	virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agent) override;
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
	virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agent) override;

	void SetWanderOffset(float offset) { m_Offset = offset; };
	void SetWanderRadius(float radius) { m_Radius = radius; };

protected:
	float m_WalkDelta = 0.f;
	const float m_DeltaMultiplier = 0.15f;
	float m_Offset = 8.f;
	float m_Radius = 4.f;
};

///////////////////////////////////////
//FLEE
//****
class Flee : public Seek
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//Seek Behaviour
	virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agent) override;
	void SetFleeRadius(float radius) { m_FleeRadius = radius; }

protected:
	const TargetData* m_pTargetRef = nullptr;
	float m_FleeRadius = 30.f;
};
