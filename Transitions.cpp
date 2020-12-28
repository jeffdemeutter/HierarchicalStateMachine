#include "stdafx.h"
#include "Transitions.h"

bool HouseInFov::ToTransition(Blackboard* pBlackboard) const
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

bool ItemInFov::ToTransition(Blackboard* pBlackboard) const
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

bool NoItemInFov::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pInterface) return false;

	UINT idx = 0;
	EntityInfo entityInfo;
	return !pInterface->Fov_GetEntityByIndex(idx, entityInfo);
}

bool VisitedHouse::ToTransition(Blackboard* pBlackboard) const
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

bool IsInVisitedHouse::ToTransition(Blackboard* pBlackboard) const
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

bool IsNotInHouse::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pInterface) return false;

	return !pInterface->Agent_GetInfo().IsInHouse;
}

bool OutsideMap::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pInterface) return false;

	const WorldInfo map = pInterface->World_GetInfo();
	Vector2 pos = pInterface->Agent_GetInfo().Position;
	Abs(pos);
	return ((pos.x > map.Dimensions.x / 2) || (pos.y > map.Dimensions.y / 2));
}

bool InsideMap::ToTransition(Blackboard* pBlackboard) const
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

bool EnemyInFov::ToTransition(Blackboard* pBlackboard) const
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return false;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pAgent || !pInterface) return false;

	bool found{ false };
	EntityInfo entityInfo;
	vector<EnemyInfo> enemyVec;
	for (int i = 0;; ++i)
	{
		// if no entity is found, break
		if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
			break;

		// if entity is not an item go to next entity
		if (entityInfo.Type != eEntityType::ENEMY)
			continue;

		EnemyInfo enemyInfo{};
		pInterface->Enemy_GetInfo(entityInfo, enemyInfo);
		
		enemyVec.push_back(enemyInfo);
		found = true;
	}
	pBlackboard->ChangeData("enemyVec", enemyVec);
	pBlackboard->ChangeData("timer", 1.5f);
	pAgent->ResetTimer();

	return found;
}

bool NoEnemyInFov::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pInterface) return false;

	EntityInfo entityInfo;
	return !pInterface->Fov_GetEntityByIndex(0, entityInfo);
}

bool Timer::ToTransition(Blackboard* pBlackboard) const
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	float timer = 0.f;
	if (!pBlackboard->GetData("agent", pAgent)) return false;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pBlackboard->GetData("timer", timer)) return false;
	if (!pAgent || !pInterface) return false;

	return pAgent->GetTimer() > timer;
}

bool HasToEat::ToTransition(Blackboard* pBlackboard) const
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return false;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	if (pInterface->Agent_GetInfo().Energy );
} 
