#include "stdafx.h"
#include "Transitions.h"

bool HouseInFov::ToTransition(Blackboard* pBlackboard) const
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent))	return false;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	HouseInfo houseInfo;
	if (pInterface->Fov_GetHouseByIndex(0, houseInfo))
	{
		if (pAgent->CheckVisitedRecently(houseInfo))
			return false;

		pBlackboard->ChangeData("house", houseInfo);
		return true;
	}
}

bool ItemInFov::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pInterface) return false;

	EntityInfo entityInfo;
	deque<EntityInfo> vEntities;
	for (int i = 0;; ++i)
	{
		// if no entity is found, break
		if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
			break;

		// if entity is not an item go to next entity
		if (entityInfo.Type != eEntityType::ITEM)
			continue;

		vEntities.push_back(entityInfo);
		pBlackboard->ChangeData("vItems", vEntities);
		return true;
	}

	return false;
}

bool PickedUpAll::ToTransition(Blackboard* pBlackboard) const
{
	deque<EntityInfo> vEntities;
	pBlackboard->GetData("vItems", vEntities);

	return (vEntities.size() == 0);
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

	// distancesqr 10.f is roughly 3.f distance
	if (DistanceSquared(pInterface->Agent_GetInfo().Position, house.Center) < 10.f)
	{
		pBlackboard->ChangeData("target", house.Center + house.Size);
		pAgent->SetHouseToVisited(house);
		return true;
	}

	return false;
}

bool IsInHouse::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pInterface) return false;

	return (pInterface->Agent_GetInfo().IsInHouse);
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

		pBlackboard->ChangeData("enemy", enemyInfo);
		pBlackboard->ChangeData("timer", 0.5f);
		pAgent->ResetTimer();
		return true;
	}
	return false;
}

bool NoEnemyInFov::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	// loop over the enemies inside the fov
	EntityInfo entityInfo{};
	for (int i = 0;; ++i)
	{
		// no (more) entities in fov
		if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
			return true;

		// if the entity is an enemy
		if (entityInfo.Type == eEntityType::ENEMY)
			return false;

		continue;
	}

}

bool Timer::ToTransition(Blackboard* pBlackboard) const
{
	SteeringAgent* pAgent = nullptr;
	float timer = 0.f;
	if (!pBlackboard->GetData("agent", pAgent)) return false;
	if (!pBlackboard->GetData("timer", timer)) return false;

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

	//check for the first gun
	ItemInfo itemInfo{};
	int bullets{ 0 };
	if (pInterface->Inventory_GetItem(0, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);
	// check for the second gun
	if (pInterface->Inventory_GetItem(1, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);

	if (bullets == 0)
		return false;

	if (pInterface->Agent_GetInfo().WasBitten)
	{
		pBlackboard->ChangeData("timer", 1.2f);
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

bool HasNoBullets::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	//check for the first gun
	ItemInfo itemInfo{};
	int bullets{ 0 };
	if (pInterface->Inventory_GetItem(0, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);
	// check for the second gun
	if (pInterface->Inventory_GetItem(1, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);

	return (bullets == 0);
}

bool CanKillEnemies::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	//check for the first gun
	ItemInfo itemInfo{};
	int bullets{ 0 };
	if (pInterface->Inventory_GetItem(0, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);
	// check for the second gun
	if (pInterface->Inventory_GetItem(1, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);

	return (bullets != 0);
}

bool InPurgeZone::ToTransition(Blackboard* pBlackboard) const
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return false;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	EntityInfo entityInfo;
	for (int i = 0;; ++i)
	{
		// if no entity is found, break
		if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
			break;

		// if entity is not an enemy go to next entity
		if (entityInfo.Type != eEntityType::PURGEZONE)
			continue;

		PurgeZoneInfo PurgeZoneInfo{};
		pInterface->PurgeZone_GetInfo(entityInfo, PurgeZoneInfo);

		pBlackboard->ChangeData("purge", PurgeZoneInfo);
		return true;
	}
}

bool NotInPurge::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;
	if (!pInterface) return false;

	// loop over the enemies inside the fov
	EntityInfo entityInfo{};
	for (int i = 0;; ++i)
	{
		// no (more) entities in fov
		if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
			return true;

		// if the entity is an enemy
		if (entityInfo.Type == eEntityType::PURGEZONE)
			return false;

		continue;
	}
}

bool CannotKill::ToTransition(Blackboard* pBlackboard) const
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return false;

	//check for the first gun
	ItemInfo itemInfo{};
	int bullets{ 0 };
	if (pInterface->Inventory_GetItem(0, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);
	// check for the second gun
	if (pInterface->Inventory_GetItem(1, itemInfo))
		bullets += pInterface->Weapon_GetAmmo(itemInfo);

	return (bullets == 0);
}
