#include "stdafx.h"
#include "States.h"

void WanderState::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pAgent) return;

	pAgent->SetToWander();
}

void ReturnToMap::Update(Blackboard* pBlackboard, float deltaTime)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pAgent || !pInterface) return;

	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(pInterface->World_GetInfo().Center));
}

void EnterHouse::Update(Blackboard* pBlackboard, float deltaTime)
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

void EnterHouse::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	if (!pBlackboard->GetData("agent", pAgent))	return;
	pAgent->CanRun(true);
}

void EnterHouse::OnExit(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	HouseInfo house{};
	if (!pBlackboard->GetData("agent", pAgent))	return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("house", house)) return;

	if (!pAgent || !pInterface) return;

	pAgent->CanRun(false);

	if(pInterface->Agent_GetInfo().IsInHouse)
		pAgent->SetHouseToVisited(house);
}

void EscapeHouse::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	HouseInfo house{};
	if (!pBlackboard->GetData("agent", pAgent))	return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("house", house)) return;

	if (!pAgent || !pInterface) return;

	Vector2 temp = pInterface->Agent_GetInfo().Position - house.Center;
	temp.x /= temp.x;
	temp.y /= temp.y;
	temp = temp * house.Size;

	pBlackboard->ChangeData("target", house.Center + temp);
}

void EscapeHouse::Update(Blackboard* pBlackboard, float deltaTime)
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

void PickUpItem::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	deque<EntityInfo> vEntities{};
	if (!pBlackboard->GetData("agent", pAgent))	return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("vItems", vEntities)) return;

	if (!pAgent || !pInterface) return;

	pAgent->SetToSeek(vEntities[0].Location);
}

void PickUpItem::Update(Blackboard* pBlackboard, float deltaTime)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	deque<EntityInfo> vEntities{};
	if (!pBlackboard->GetData("agent", pAgent))	return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("vItems", vEntities)) return;

	if (!pInterface) return;

	// check for new items in fov

	const Vector2 agentPos{ pInterface->Agent_GetInfo().Position };

	if (vEntities.size() == 0) return;
	pAgent->SetToSeek(vEntities[0].Location);

	EntityInfo entityInfo;
	for (int i = 0;; ++i)
	{
		// if no entity is found, break
		if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
			break;

		// if entity is not an item go to next entity
		if (entityInfo.Type != eEntityType::ITEM)
			continue;

		// checks if that item is already in the deque
		auto found = std::find_if(vEntities.begin(), vEntities.end(), [&entityInfo](const auto& e) {
				return entityInfo.EntityHash == e.EntityHash;
			});

		// if in deck, go to next item in fov
		if (found != vEntities.end())
			continue;

		// if not in deque, add it
		vEntities.push_back(entityInfo);
		continue;
	}
	// update the deque with possible new items
	std::sort(vEntities.begin(), vEntities.end(), [&agentPos](const EntityInfo& a, const EntityInfo& b) {
		return (Distance(agentPos, a.Location) < Distance(agentPos, b.Location));
	});
	pBlackboard->ChangeData("vItems", vEntities);

	// check if its in grab range
	if (Distance(vEntities[0].Location, agentPos) > pInterface->Agent_GetInfo().GrabRange)
		return;

	// replace the item
	ItemInfo pickedUpItem{};
	if (pInterface->Item_Grab(vEntities[0], pickedUpItem))
	{
		vEntities.pop_front();
		pBlackboard->ChangeData("vItems", vEntities);

		// if item is a pistol
		if (eItemType::PISTOL == pickedUpItem.Type)
		{
			// if the slots are empty, just insert them 
			if (pInterface->Inventory_AddItem(0, pickedUpItem)) return;
			if (pInterface->Inventory_AddItem(1, pickedUpItem)) return;

			// otherwise check if the new pistol has more ammo then the current ones in inventory
			const int newPistolAmmo{ pInterface->Weapon_GetAmmo(pickedUpItem) };
			ItemInfo pistol{};
			// pistol 1 ammo (ne need for an if statement since we alrady return if there are no pistols in the slots yet)
				pInterface->Inventory_GetItem(0, pistol);
			const int weaponAmmo1 = pInterface->Weapon_GetAmmo(pistol);
			// pistol 2 ammo
				pInterface->Inventory_GetItem(1, pistol);
			const int weaponAmmo2 = pInterface->Weapon_GetAmmo(pistol);

			// checks if found pistol has more ammo then the current ones
			if (newPistolAmmo > weaponAmmo1 || newPistolAmmo > weaponAmmo2)
			{
				// switch lowest ammo first
				if (weaponAmmo1 < weaponAmmo2)
				{
					pInterface->Inventory_RemoveItem(0);
					if (pInterface->Inventory_AddItem(0, pickedUpItem)) return;
				}
				else
				{
					pInterface->Inventory_RemoveItem(1);
					if (pInterface->Inventory_AddItem(1, pickedUpItem)) return;
				}
			}
		}
		// checks if the item is a medkit
		else if (eItemType::MEDKIT == pickedUpItem.Type)
		{
			if (pInterface->Inventory_AddItem(2, pickedUpItem)) return;
		}
		// checks if the item is food
		else if (eItemType::FOOD == pickedUpItem.Type)
		{
			if (pInterface->Inventory_AddItem(3, pickedUpItem)) return;

			// get food energy amt
			ItemInfo food{};
				pInterface->Inventory_GetItem(3, food);
			if (pInterface->Food_GetEnergy(pickedUpItem) > pInterface->Food_GetEnergy(food))
			{
				pInterface->Inventory_RemoveItem(3);
				if (pInterface->Inventory_AddItem(3, pickedUpItem)) return;
			}
		}

		pInterface->Inventory_AddItem(4, pickedUpItem);
		pInterface->Inventory_RemoveItem(4);
	}
}

void EvadeState::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pAgent || !pInterface) return;

	// Get enemies in current vision
	EntityInfo entityInfo;
	vector<EnemyInfo> enemyVec{};
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

		enemyVec.push_back(enemyInfo);
	}
	pBlackboard->ChangeData("enemyVec", enemyVec);
	// reset timer
	pAgent->CanRun(true);
}

void EvadeState::OnExit(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	pAgent->CanRun(false);
}

void EvadeState::Update(Blackboard* pBlackboard, float deltaTime)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	vector<EnemyInfo> enemyVec{};
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("enemyVec", enemyVec)) return;
	if (!pAgent || !pInterface) return;


	static const float distCheck{ 8.f };
	const float angle{ pInterface->Agent_GetInfo().FOV_Angle / 2.f };
	const Vector2 agentPos = pInterface->Agent_GetInfo().Position;
	// calculate the three points to check if theyre safe
	const Vector2 dPointMiddle = GetNormalized(pInterface->Agent_GetInfo().LinearVelocity) * pInterface->Agent_GetInfo().FOV_Range;
	const Vector2 dPointLeft = Vector2{ dPointMiddle.x * cosf(-angle) - dPointMiddle.y * sinf(-angle)	, dPointMiddle.x * sinf(-angle) + dPointMiddle.y * cosf(-angle) };
	const Vector2 dPointRight = Vector2{ dPointMiddle.x * cosf(angle) - dPointMiddle.y * sinf(angle)	, dPointMiddle.x * sinf(angle) + dPointMiddle.y * cosf(angle) };

	// check which is a safe route
	bool isLeftSafe = true;
	bool isMiddleSafe = true;
	bool isRightSafe = true;
	for (auto& enemy : enemyVec)
	{
		if (isLeftSafe && Distance(enemy.Location, agentPos + dPointLeft) < distCheck)
			isLeftSafe = false;
		if (isMiddleSafe && Distance(enemy.Location, agentPos + dPointMiddle) < distCheck)
			isMiddleSafe = false;
		if (isRightSafe && Distance(enemy.Location, agentPos + dPointRight) < distCheck)
			isRightSafe = false;
	}

	Vector2 target{agentPos};
	// times 5.f just to be sure since it uses a timer transition 
	 if (isMiddleSafe)
		target += dPointMiddle * 5.f;
	else if (isLeftSafe)
		target += dPointLeft * 5.f;
	else if (isRightSafe)
		target += dPointRight * 5.f;
	else
		target -= dPointMiddle * 5.f;

	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(target));
}

void Eat::OnEnter(Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return;

	pInterface->Inventory_UseItem(3);
	pInterface->Inventory_RemoveItem(3);
}

void KillFollowers::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;

	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(pInterface->Agent_GetInfo().Position + pInterface->Agent_GetInfo().LinearVelocity));

	pAgent->CanRun(true);
}

void KillFollowers::OnExit(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;

	pAgent->CanRun(false);
}

void Heal::OnEnter(Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("interface", pInterface)) return;

	pInterface->Inventory_UseItem(2);
	pInterface->Inventory_RemoveItem(2);
}

void Shoot::Update(Blackboard* pBlackboard, float deltaTime)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	EnemyInfo enemyInfo{};
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("enemy", enemyInfo)) return;
	if (!pInterface) return;

	pAgent->SetToRotate(enemyInfo.Location);

	pInterface->Draw_Segment(pInterface->Agent_GetInfo().Position, enemyInfo.Location, { 1.f, 0.f, 0.f });
	pInterface->Draw_Segment(pInterface->Agent_GetInfo().Position, pInterface->Agent_GetInfo().Position + pInterface->Agent_GetInfo().LinearVelocity, { 0.f, 1.f, 0.f });

	EntityInfo entityInfo{};
	for (int i = 0;; ++i)
	{
		if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
			break;

		if (entityInfo.Type != eEntityType::ENEMY)
			continue;

		EnemyInfo enemy{};
		pInterface->Enemy_GetInfo(entityInfo, enemy);
		pBlackboard->ChangeData("enemy", enemy);

		break;
	}

	const float rot{ pInterface->Agent_GetInfo().Orientation - float(E_PI_2) };
	// direction vector of agent
	const Vector2 forward{ GetNormalized(Vector2{cosf(rot), sinf(rot)}) };
	// vector from agent towards enemy
	const Vector2 toEnemy{ GetNormalized(enemyInfo.Location - pInterface->Agent_GetInfo().Position) };
	const float canShoot{ Dot(toEnemy, forward) };
	
	pInterface->Draw_Segment(pInterface->Agent_GetInfo().Position, pInterface->Agent_GetInfo().Position + (forward * pInterface->Agent_GetInfo().MaxLinearSpeed), { 0.0f, 1.f, 0.f });
	pInterface->Draw_Segment(pInterface->Agent_GetInfo().Position, pInterface->Agent_GetInfo().Position + (toEnemy * pInterface->Agent_GetInfo().MaxLinearSpeed), {1.0f, 0.f, 0.f});

	const float dist = Distance(pInterface->Agent_GetInfo().Position, enemyInfo.Location);

	if (canShoot > 0.999f)
	{
		ItemInfo pistol{};
		// pistol 1 ammo
		int weaponAmmo1{ 0 };
		int weaponAmmo2{ 0 };
		pInterface->Inventory_GetItem(0, pistol);
		weaponAmmo1 = pInterface->Weapon_GetAmmo(pistol);
		if (weaponAmmo1 == 0)
			pInterface->Inventory_RemoveItem(0);
		// pistol 2 ammo
		pInterface->Inventory_GetItem(1, pistol);
		weaponAmmo2 = pInterface->Weapon_GetAmmo(pistol);
		if (weaponAmmo2 == 0)
			pInterface->Inventory_RemoveItem(1);

		if (weaponAmmo1 != 0)
			pInterface->Inventory_UseItem(0);
		else if (weaponAmmo2 != 0)
			pInterface->Inventory_UseItem(1);
	}
}

void EscapePurge::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;

	pAgent->CanRun(true);
}

void EscapePurge::OnExit(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;

	pAgent->CanRun(false);
}

void EscapePurge::Update(Blackboard* pBlackboard, float deltaTime)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	PurgeZoneInfo purgeZone{};
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("purge", purgeZone)) return;

	// times 3.f to make sure it is far enough
	const Vector2 target = (pInterface->Agent_GetInfo().Position - purgeZone.Center);

	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint( target ));
}
