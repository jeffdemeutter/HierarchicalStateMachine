/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "EFiniteStateMachine.h"
#include "SteeringAgent.h"
#include "EBlackboard.h"

using namespace Elite;

//------------------------------------------------
// Nested States
//------------------------------------------------

class Passive : public FSMState
{
public:
	Passive() : FSMState() {};
};


//------------------------------------------------
// States
//------------------------------------------------
class WanderState final : public FSMState
{
public:
	WanderState() : FSMState() {};
	virtual void OnEnter(Blackboard* pBlackboard) override
	{
		SteeringAgent* pAgent = nullptr;
		if (!pBlackboard->GetData("agent", pAgent)) return;
		if (!pAgent) return;

		pAgent->SetToWander();
	}
};

class ReturnToMap final : public FSMState
{
public:
	ReturnToMap() : FSMState() {};
	virtual void OnEnter(Blackboard* pBlackboard) override
	{
		SteeringAgent* pAgent = nullptr;
		Vector2 target;
		if (!pBlackboard->GetData("agent", pAgent)) return;
		if (!pBlackboard->GetData("target", target)) return;
		if (!pAgent) return;

		pAgent->SetToSeek(target);
	}
};

class EnterHouse : public FSMState
{
public:
	EnterHouse() : FSMState() {}
	
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override
	{
		SteeringAgent* pAgent = nullptr;
		IExamInterface* pInterface = nullptr;
		HouseInfo house{};
		if (!pBlackboard->GetData("agent", pAgent))	return;
		if (!pBlackboard->GetData("interface", pInterface)) return;
		if (!pBlackboard->GetData("house", house)) return;

		if (!pAgent || !pInterface) return;

		pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(house.Center));
	}
};

class EscapeHouse : public FSMState
{
public:
	EscapeHouse() : FSMState() {}

	virtual void Update(Blackboard* pBlackboard, float deltaTime) override
	{
		SteeringAgent* pAgent = nullptr;
		IExamInterface* pInterface = nullptr;
		Vector2 target{};
		if (!pBlackboard->GetData("agent", pAgent))	return;
		if (!pBlackboard->GetData("interface", pInterface)) return;
		if (!pBlackboard->GetData("target", target)) return;

		if (!pAgent || !pInterface) return;

		pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(target));
	}
};

class PickUpItem : public FSMState
{
public:
	PickUpItem() : FSMState() {}

	virtual void OnEnter(Blackboard* pBlackboard) override
	{
		SteeringAgent* pAgent = nullptr;
		IExamInterface* pInterface = nullptr;
		EntityInfo entityInfo{};
		if (!pBlackboard->GetData("agent", pAgent))	return;
		if (!pBlackboard->GetData("interface", pInterface)) return;
		if (!pBlackboard->GetData("entity", entityInfo)) return;

		if (!pAgent || !pInterface) return;

		pAgent->SetToSeek(entityInfo.Location);
	}

	virtual void Update(Blackboard* pBlackboard, float deltaTime) override
	{
		IExamInterface* pInterface = nullptr;
		EntityInfo itemInfo{};
		if (!pBlackboard->GetData("interface", pInterface)) return;
		if (!pBlackboard->GetData("entity", itemInfo)) return;

		if (!pInterface) return;
		
		if (Distance(itemInfo.Location, pInterface->Agent_GetInfo().Position) > pInterface->Agent_GetInfo().GrabRange)
			return;

		ItemInfo pickedUpItem{};
		if (pInterface->Item_Grab(itemInfo, pickedUpItem))
		{
			if (eItemType::PISTOL == pickedUpItem.Type)
			{
				if (pInterface->Inventory_AddItem(0, pickedUpItem)) return;
			}
			else if (eItemType::MEDKIT == pickedUpItem.Type)
			{
				if (pInterface->Inventory_AddItem(1, pickedUpItem)) return;
				if (pInterface->Inventory_AddItem(2, pickedUpItem)) return;
			}
			else if (eItemType::FOOD == pickedUpItem.Type)
			{
				if (pInterface->Inventory_AddItem(3, pickedUpItem)) return;
			}

			pInterface->Inventory_AddItem(4, pickedUpItem);
			pInterface->Inventory_RemoveItem(4);
		}
	}
};

//---------------------------------------------------------------------------------------------------
// Transitions
//---------------------------------------------------------------------------------------------------
class HouseInFov : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		SteeringAgent* pAgent = nullptr;
		IExamInterface* pInterface = nullptr;
		if (!pBlackboard->GetData("agent", pAgent))	return false;
		if (!pBlackboard->GetData("interface", pInterface)) return false;
		if (!pInterface) return false;

		UINT idx = 0;
		HouseInfo houseInfo;
		if (pInterface->Fov_GetHouseByIndex(idx, houseInfo))
		{
			if (pAgent->CheckVisitedRecently(houseInfo))
				return false;

			pBlackboard->ChangeData("house", houseInfo);

			return true;
		}

		return false;
	}
};

class ItemInFov : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		IExamInterface* pInterface = nullptr;
		if (!pBlackboard->GetData("interface", pInterface)) return false;
		if (!pInterface) return false;

		EntityInfo entityInfo;
		for (int i = 0;; ++i)
		{
			// if no entity is found, break
			if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
				break;
			
			// if entity is not an item go to next entity
			if (entityInfo.Type != eEntityType::ITEM)
				continue;

			pBlackboard->ChangeData("entity", entityInfo);
			pBlackboard->ChangeData("target", entityInfo.Location);

			return true;
		}		

		return false;
	}
};

class NoItemInFov : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		IExamInterface* pInterface = nullptr;
		if (!pBlackboard->GetData("interface", pInterface)) return false;
		if (!pInterface) return false;

		UINT idx = 0;
		EntityInfo entityInfo;
		return !pInterface->Fov_GetEntityByIndex(idx, entityInfo); 
	}
};

class VisitedHouse : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		SteeringAgent* pAgent = nullptr;
		IExamInterface* pInterface = nullptr;
		HouseInfo house;
		if (!pBlackboard->GetData("agent", pAgent))	return false;
		if (!pBlackboard->GetData("interface", pInterface)) return false;
		if (!pBlackboard->GetData("house", house)) return false;
		if (!pAgent || !pInterface) return false;
		
		if (Distance(pInterface->Agent_GetInfo().Position, house.Center) < 2.f)
		{
			pBlackboard->ChangeData("target", house.Center + house.Size);
			pAgent->SetToVisited(house);
			return true;
		}
		
		return false;
	}
};

class IsInVisitedHouse : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		SteeringAgent* pAgent = nullptr;
		IExamInterface* pInterface = nullptr;
		HouseInfo house;
		if (!pBlackboard->GetData("agent", pAgent))	return false;
		if (!pBlackboard->GetData("interface", pInterface)) return false;
		if (!pBlackboard->GetData("house", house)) return false;
		if (!pAgent || !pInterface) return false;

		if (pInterface->Agent_GetInfo().IsInHouse)
		{
			pBlackboard->ChangeData("target", house.Center + house.Size);
			return true;
		}

		return false;
	}
};

class IsNotInHouse : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		IExamInterface* pInterface = nullptr;
		if (!pBlackboard->GetData("interface", pInterface)) return false;
		if (!pInterface) return false;

		return !pInterface->Agent_GetInfo().IsInHouse;
	}
};

class OutsideMap : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		IExamInterface* pInterface = nullptr;
		Vector2 target;
		if (!pBlackboard->GetData("interface", pInterface)) return false;
		if (!pBlackboard->GetData("target", target)) return false;
		if (!pInterface) return false;

		WorldInfo map = pInterface->World_GetInfo();
		Vector2 pos = pInterface->Agent_GetInfo().Position;
		Abs(pos);
		return ((pos.x > map.Dimensions.x/2) || (pos.y > map.Dimensions.y/2));
	}
};

class InsideMap : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override
	{
		IExamInterface* pInterface = nullptr;
		Vector2 target;
		if (!pBlackboard->GetData("interface", pInterface)) return false;
		if (!pBlackboard->GetData("target", target)) return false;
		if (!pInterface) return false;

		WorldInfo map = pInterface->World_GetInfo();
		Vector2 pos = pInterface->Agent_GetInfo().Position;
		Abs(pos);
		return ((pos.x < map.Dimensions.x / 2) && (pos.y < map.Dimensions.y / 2));
	}
};

#endif