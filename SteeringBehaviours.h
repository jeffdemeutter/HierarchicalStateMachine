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

//SEEK
class Seek final : public SteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agent) override;
};

//Rotate
class Rotate final : public SteeringBehavior
{
public:
	Rotate() = default;
	virtual ~Rotate() = default;

	//Seek Behaviour
	virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agent) override;
};

