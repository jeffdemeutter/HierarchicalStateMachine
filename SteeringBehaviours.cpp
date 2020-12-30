#include "stdafx.h"

#include "SteeringBehaviours.h"

SteeringPlugin_Output Seek::CalculateSteering(float deltaT, const AgentInfo& agent)
{
	SteeringPlugin_Output steering{};

	steering.LinearVelocity = m_Target - agent.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= agent.MaxLinearSpeed;

	steering.AutoOrient = true;

	return steering;
}


SteeringPlugin_Output Wander::CalculateSteering(float deltaT, const AgentInfo& agent)
{
	// find center of circle
	Elite::Vector2 direction = agent.LinearVelocity.GetNormalized();
	if (direction == Elite::ZeroVector2)
		direction = Elite::UnitVector2;
	const Elite::Vector2 center{ agent.Position + direction * m_Offset };

	// random walk
	m_WalkDelta += (Elite::randomFloat(m_Radius * 2) - m_Radius) * m_DeltaMultiplier;
	const float x = cos(m_WalkDelta) * m_Radius;
	const float y = sin(m_WalkDelta) * m_Radius;

	m_Target = center + Elite::Vector2{ x, y };

	return Seek::CalculateSteering(deltaT, agent);
}                                                                                                                                                                                                       

SteeringPlugin_Output Flee::CalculateSteering(float deltaT, const AgentInfo& agent)
{
	SteeringPlugin_Output steering{};

	if (abs(Elite::Distance(agent.Position, m_Target)) < m_FleeRadius)
	{
		steering = Seek::CalculateSteering(deltaT, agent);
		steering.LinearVelocity *= -1;
	}

	steering.AutoOrient = true;

	return steering;
}

SteeringPlugin_Output Rotate::CalculateSteering(float deltaT, const AgentInfo& agent)
{
	// used formula by hiigara https://www.gamedev.net/forums/topic/566862-how-do-i-convert-an-angle-to-the-range-pipi/
	SteeringPlugin_Output steering{};

	const Elite::Vector2 toTarget{ m_Target - agent.Position };

	const float to{ atan2f(toTarget.y, toTarget.x) + float(E_PI_2) };
	float from = atan2f(sinf(agent.Orientation), cosf(agent.Orientation));
	float desired = to - from;

	if (desired > E_PI)
		desired -= E_TAU;
	else if (desired < -E_PI)
		desired += E_TAU;

	// multiply desired by some value to make it go as fast as possible (30.f)
	steering.AngularVelocity = desired * 30.f;

	steering.AutoOrient = false;

	return steering;
}
