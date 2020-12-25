#include "stdafx.h"
#include "SteeringAgent.h"


SteeringAgent::SteeringAgent()
	: m_pWander{ new Wander() }
	, m_pSeek{ new Seek() }
{

	SetToWander();
}

SteeringAgent::~SteeringAgent()
{
	SAFE_DELETE(m_pDecisionMaking);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pSteeringBehaviour);
}

SteeringPlugin_Output SteeringAgent::Update(float deltaT, const AgentInfo& agent)
{
	// Decide which behaviour to use
	if (m_pDecisionMaking)
		m_pDecisionMaking->Update(deltaT);

	// Calculate all steering stuff
	auto steering = m_pSteeringBehaviour->CalculateSteering(deltaT, agent);


	// steering.AngularVelocity = m_AngSpeed; //Rotate your character to inspect the world while walking
	steering.AutoOrient = true; //Setting AutoOrientate to TRue overrides the AngularVelocity

	steering.RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)
	

	// Reset Utilities
	m_GrabItem = false; 
	m_UseItem = false;
	m_RemoveItem = false;

	return steering;
}

void SteeringAgent::SetDecisionMaking(BehaviorTree* decisionMakingStructure)
{
	m_pDecisionMaking = decisionMakingStructure;
}

void SteeringAgent::SetToWander()
{
	m_pSteeringBehaviour = m_pWander;
}

void SteeringAgent::SetToSeek(const Vector2& seekTarget)
{
	m_pSteeringBehaviour->SetTarget(seekTarget);
	m_pSteeringBehaviour = m_pSeek;
}
