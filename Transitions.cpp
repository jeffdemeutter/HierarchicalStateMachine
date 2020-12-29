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
	return ((pos.x < map.Dimensions.x / 3) && (pos.y < map.Dimensions.y / 3));
}

bool EnemyInFov::ToTransition(Blackboard* pBlackboard) const
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return false;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pAgent || !pInterface) return false;

	EntityInfo entityInfo;
	for (int i = 0;; ++i)
	{
		// if no entity is found, break
		if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
			break;

		// if entity is not an enemy go to next entity
		if (entityInfo.Type != eEntityType::ENEMY)
			continue;

		EnemyInfo enemyInfo{};
		pInterface->Enemy_GetInfo(entityInfo, enemyInfo);

		pBlackboard->ChangeData("timer", 0.5f);
		pAgent->ResetTimer();
		return true;
	}
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
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	ItemInfo itemInfo{};
	if (pInterface->Inventory_GetItem(3, itemInfo))
		if (10.f - pInterface->Agent_GetInfo().Energy > pInterface->Food_GetEnergy(itemInfo))
			return true;
	
	return false;
} 

bool WasHit::ToTransition(Blackboard* pBlackboard) const
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return false;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	if (pInterface->Agent_GetInfo().WasBitten)
	{
		pBlackboard->ChangeData("timer", 3.f);
		pAgent->ResetTimer();

		return true;
	}
	return false;
}

bool HasToHeal::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	ItemInfo itemInfo{};
	if (pInterface->Inventory_GetItem(2, itemInfo))
		if (10.f - pInterface->Agent_GetInfo().Health > pInterface->Medkit_GetHealth(itemInfo))
			return true;

	return false;
}

bool GotKill::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	int kills = 0;
	pBlackboard->GetData("kills", kills);

	if (pInterface->World_GetStats().NumEnemiesKilled != kills);
	{
		pBlackboard->ChangeData("kills", pInterface->World_GetStats().NumEnemiesKilled);
		return true;
	}
	return false;
}

bool HasNoBullets::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	//check for the first gun
	ItemInfo itemInfo{};
	int bullets{ 0 };
	if (!pInterface->Inventory_GetItem(0, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);
	// check for the second gun
	if (pInterface->Inventory_GetItem(1, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);

	return (bullets < 3);
}

bool HasBullets::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	//check for the first gun
	ItemInfo itemInfo{};
	int bullets{ 0 };
	if (!pInterface->Inventory_GetItem(0, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);
	// check for the second gun
	if (pInterface->Inventory_GetItem(1, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);

	return (bullets > 3);
}
