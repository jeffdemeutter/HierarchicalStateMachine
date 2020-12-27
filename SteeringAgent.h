#pragma once

#include "SteeringBehaviours.h"
#include "EFiniteStateMachine.h"

using namespace Elite;

struct House
{
	House(Vector2 p, bool v) : pos{ p }, visited{ v }, timer{ 0.f } {}

	const Vector2 pos;
	bool visited;
	float timer;
};

class SteeringAgent final 
{
public:
	SteeringAgent();
	~SteeringAgent();
	
	SteeringPlugin_Output Update(float deltaT, const AgentInfo& agent);
	void SetDecisionMaking(FiniteStateMachine* decisionMakingStructure);

	void SetToVisited(const HouseInfo& house);
	bool CheckVisitedRecently(const HouseInfo& house);

	void SetToWander();
	void SetToSeek(const Vector2& seekTarget);
private:
	FiniteStateMachine* m_pDecisionMaking = nullptr;

	// Steering Behaviours
	SteeringBehavior* m_pSteeringBehaviour = nullptr;

	SteeringBehavior* m_pWander = nullptr;
	SteeringBehavior* m_pSeek = nullptr;

	// Utilities
	bool m_IsEnteringHouse = false;
	bool m_CanRun = false;	
	float m_AngSpeed = 0.f;

	std::vector<House> m_Houses;
};

