/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------

#include "EBehaviorTree.h"
#include "SteeringAgent.h"

using namespace Elite;

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

//bool IsCloseToFood(Elite::Blackboard* pBlackboard);

bool HouseFound(Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	pBlackboard->GetData("interface", pInterface);
	// empty interface
	if (!pInterface)
		return false;

	UINT idx = 0;
	HouseInfo houseInfo;
	if (pInterface->Fov_GetHouseByIndex(idx, houseInfo))
	{
		pBlackboard->ChangeData("target", houseInfo.Center);

		return true;
	}

	return false;
}



//-----------------------------------------------------------------
// States
//-----------------------------------------------------------------
BehaviorState ChangeToWander(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	pBlackboard->GetData("agent", pAgent);

	if (!pAgent)
		return Failure;

	std::cout << "Wandering\n"; 
	pAgent->SetToWander();

	return Success;
}

BehaviorState ChangeToSeek(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	Vector2 seekTarget{};
	pBlackboard->GetData("agent", pAgent);
	pBlackboard->GetData("interface", pInterface);
	pBlackboard->GetData("target", seekTarget);

	if (!pAgent || !pInterface)
		return Failure;

	std::cout << "Seeking\n"; 
	seekTarget = pInterface->NavMesh_GetClosestPathPoint(seekTarget);

	pAgent->SetToSeek(seekTarget);

	return Success;
}

BehaviorState ChangeToEvade(Blackboard* pBlackboard)
{
	//AgarioAgent* pAgent = nullptr;
	//Vector2 evadeTarget{};
	//auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
	//	pBlackboard->GetData("Target", evadeTarget);

	//if (!pAgent)
	//	return Failure;

	////TODO: Implement Change to evade (Target)
	//std::cout << "Evading\n"; 
	//pAgent->SetToEvade(evadeTarget);


	return Success;
}

#endif