#include "stdafx.h"
#include "SteeringAgent.h"


SteeringAgent::SteeringAgent()
	: m_pWander{ new Wander() }
	, m_pSeek{ new Seek() }
{

	SetToWander();

	m_pWander = new Wander();
	m_pSeek = new Seek();
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

void SteeringAgent::SetToWander()
{
	m_pSteeringBehaviour = m_pWander;
}

void SteeringAgent::SetToSeek(const Vector2& seekTarget)
{
	m_pSteeringBehaviour = m_pSeek;
	m_pSteeringBehaviour->SetTarget(seekTarget);
}
