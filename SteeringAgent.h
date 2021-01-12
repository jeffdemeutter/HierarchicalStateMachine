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

	void SetHouseToVisited(const HouseInfo& house);
	bool CheckVisitedRecently(const HouseInfo& house);

	float GetTimer() const;
	void ResetTimer();
	void CanRun(bool canRun);

	void SetToSeek(const Vector2& seekTarget);
	void SetToRotate(const Vector2& rotateTowards);
private:
	FiniteStateMachine* m_pDecisionMaking = nullptr;

	// Steering Behaviours
	SteeringBehavior* m_pSteeringBehaviour = nullptr;

	SteeringBehavior* m_pSeek	= nullptr;
	SteeringBehavior* m_pRotate	= nullptr;

	// Utilities
	bool m_CanRun = false;	
	float m_AngSpeed = 0.f;

	float m_Timer = 0.f;

	std::vector<House> m_vHouses;
};

