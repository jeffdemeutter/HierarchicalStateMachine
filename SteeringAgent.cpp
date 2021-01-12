#include "stdafx.h"
#include "SteeringAgent.h"


SteeringAgent::SteeringAgent()
	: m_pSeek{ new Seek() }
	, m_pRotate{ new Rotate() }
{
}

SteeringAgent::~SteeringAgent()
{
	SAFE_DELETE(m_pDecisionMaking);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pRotate);
	SAFE_DELETE(m_pSteeringBehaviour);
}

SteeringPlugin_Output SteeringAgent::Update(float deltaT, const AgentInfo& agent)
{
	m_Timer += deltaT;
	for (auto& house : m_vHouses)
	{
		if (house.visited)
		{
			house.timer += deltaT;
			if (house.timer > 50.f)
			{
				house.visited = false;
				house.timer = 0.f;
			}
		}
	}

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

void SteeringAgent::SetHouseToVisited(const HouseInfo& house)
{
	auto found = std::find_if(m_vHouses.begin(), m_vHouses.end(), [&house](const auto& h) { return (house.Center == h.pos); });
	if (found != m_vHouses.end())
		found->visited = true;
}

bool SteeringAgent::CheckVisitedRecently(const HouseInfo& house)
{
	auto found = std::find_if(m_vHouses.begin(), m_vHouses.end(), [&house](const auto& h) { return (house.Center == h.pos); });
	
	if (found == m_vHouses.end())
	{
		m_vHouses.push_back(House{ house.Center, false });
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

void SteeringAgent::SetToSeek(const Vector2& seekTarget)
{
	m_pSteeringBehaviour = m_pSeek;
	m_pSteeringBehaviour->SetTarget(seekTarget);
}


void SteeringAgent::SetToRotate(const Vector2& rotateTowards)
{
	m_pSteeringBehaviour = m_pRotate;
	m_pSteeringBehaviour->SetTarget(rotateTowards);
}
