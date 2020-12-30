#include "stdafx.h"
#include "SteeringAgent.h"


SteeringAgent::SteeringAgent()
	: m_pWander{ new Wander() }
	, m_pSeek{ new Seek() }
	, m_pFlee{ new Flee() }
	, m_pRotate{ new Rotate() }
{
	SetToWander();
}

SteeringAgent::~SteeringAgent()
{
	SAFE_DELETE(m_pDecisionMaking);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pFlee);
	SAFE_DELETE(m_pRotate);
	SAFE_DELETE(m_pSteeringBehaviour);
}

SteeringPlugin_Output SteeringAgent::Update(float deltaT, const AgentInfo& agent)
{
	m_Timer += deltaT;

	// Decide which behaviour to use
	if (m_pDecisionMaking)
		m_pDecisionMaking->Update(deltaT);

	// Calculate all steering stuff
	auto steering = m_pSteeringBehaviour->CalculateSteering(deltaT, agent);


	// steering.AngularVelocity = m_AngSpeed; //Rotate your character to inspect the world while walking

	steering.RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

	return steering;
}

void SteeringAgent::SetDecisionMaking(FiniteStateMachine* decisionMakingStructure)
{
	m_pDecisionMaking = decisionMakingStructure;
}

void SteeringAgent::SetToVisited(const HouseInfo& house)
{
	auto found = std::find_if(m_Houses.begin(), m_Houses.end(), [&house](const auto& h) { return (house.Center == h.pos); });
	if (found != m_Houses.end())
		found->visited = true;
}

bool SteeringAgent::CheckVisitedRecently(const HouseInfo& house)
{
	auto found = std::find_if(m_Houses.begin(), m_Houses.end(), [&house](const auto& h) { return (house.Center == h.pos); });
	
	if (found == m_Houses.end())
	{
		m_Houses.push_back(House{ house.Center, true });
		return false;
	}

	return found->visited;
}

float SteeringAgent::GetTimer() const
{
	return m_Timer;
}

void SteeringAgent::ResetTimer()
{
	m_Timer = 0.f;
}

void SteeringAgent::CanRun(bool canRun)
{
	m_CanRun = canRun;
}

void SteeringAgent::SetToWander()
{
	m_pSteeringBehaviour = m_pWander;
}

void SteeringAgent::SetToSeek(const Vector2& seekTarget)
{
	m_pSteeringBehaviour = m_pSeek;
	m_pSteeringBehaviour->SetTarget(seekTarget);
}

void SteeringAgent::SetToFlee(const Vector2& evadeTarget)
{
	m_pSteeringBehaviour = m_pFlee;
	m_pSteeringBehaviour->SetTarget(evadeTarget);
}

void SteeringAgent::SetToRotate(const Vector2& rotateTowards)
{
	m_pSteeringBehaviour = m_pRotate;
	m_pSteeringBehaviour->SetTarget(rotateTowards);
}
