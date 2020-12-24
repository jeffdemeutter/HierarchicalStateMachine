#include "stdafx.h"

#include "SteeringBehaviours.h"

SteeringPlugin_Output Seek::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering{};

	steering.LinearVelocity = m_Target - pAgent.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent.MaxLinearSpeed;

	return steering;
}

SteeringPlugin_Output Wander::CalculateSteering(float deltaT, AgentInfo pAgent)
{
	SteeringPlugin_Output steering{};

	const Elite::Vector2 circlePos = pAgent.Position + (pAgent.LinearVelocity.GetNormalized() * m_Offset);
	Elite::Vector2 newPos;

	m_WanderAngle += Elite::randomFloat(m_AngleChange * 2) - m_WanderAngle;
	newPos.x = sin(m_WanderAngle) * m_Radius + circlePos.x;
	newPos.y = cos(m_WanderAngle) * m_Radius + circlePos.y;

	m_Target = newPos;

	steering.LinearVelocity = newPos - pAgent.Position;

	return steering;
}
