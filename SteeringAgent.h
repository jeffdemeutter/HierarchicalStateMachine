#pragma once

#include "SteeringBehaviours.h"
#include "EBehaviorTree.h"

using namespace Elite;


class SteeringAgent final 
{
public:
	SteeringAgent();
	~SteeringAgent();

	
	SteeringPlugin_Output Update(float deltaT, const AgentInfo& agent);
	void SetDecisionMaking(BehaviorTree* decisionMakingStructure);

	void SetToWander();
	void SetToEnterHouse();
	void SetToSeek(const Vector2& seekTarget);
private:
	BehaviorTree* m_pDecisionMaking = nullptr;

	// Steering Behaviours
	Wander* m_pWander = nullptr;
	Seek* m_pSeek = nullptr;

	SteeringBehavior* m_pSteeringBehaviour = nullptr;

	// Utilities
	bool m_CanRun = false;	
	bool m_GrabItem = false;
	bool m_UseItem = false;	
	bool m_RemoveItem = false;
	float m_AngSpeed = 0.f;

};

