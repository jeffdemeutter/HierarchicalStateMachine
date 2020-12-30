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

	ItemInfo pickedUpItem{};
	if (pInterface->Item_Grab(vEntities[0], pickedUpItem))
	{
		vEntities.pop_front();
		pBlackboard->ChangeData("vItems", vEntities);

		if (eItemType::PISTOL == pickedUpItem.Type)
		{
			if (pInterface->Inventory_AddItem(0, pickedUpItem)) return;
			if (pInterface->Inventory_AddItem(1, pickedUpItem)) return;
		}
		else if (eItemType::MEDKIT == pickedUpItem.Type)
		{
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

void RunAway::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;

	pAgent->CanRun(true);
}

void RunAway::Update(Blackboard* pBlackboard, float deltaTime)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;

	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(pInterface->Agent_GetInfo().Position + pInterface->Agent_GetInfo().LinearVelocity * 3.f));
}

void RunAway::OnExit(Blackboard* pBlackboard)
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

void Shoot::OnEnter(Blackboard* pBlackboard)
{
	m_Timer = 0.f;
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

	//EntityInfo entityInfo{};
	//for (int i = 0;; ++i)
	//{
	//	if (pInterface->Fov_GetEntityByIndex(i, entityInfo))
	//		break;

	//	if (entityInfo.Type != eEntityType::ENEMY)
	//		continue;

	//	pInterface->Enemy_GetInfo(entityInfo, enemyInfo);

	//	pBlackboard->ChangeData("enemy", enemyInfo);
	//}

	m_Timer += deltaTime;

	if (m_Timer > 0.5f)
	{
		UINT idx = 0;
		ItemInfo itemInfo{};
		if (!pInterface->Inventory_GetItem(idx, itemInfo))
			idx = 1;
		else if (!pInterface->Inventory_GetItem(idx, itemInfo))
			return;

		if (pInterface->Weapon_GetAmmo(itemInfo) != 0)
			pInterface->Inventory_UseItem(idx);
		else
			pInterface->Inventory_RemoveItem(idx);

		// reset timer
		m_Timer = 0.f;
	}
}

void TurnAround::Update(Blackboard* pBlackboard, float deltaTime)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;

	pAgent->SetToSeek(pInterface->Agent_GetInfo().Position - GetNormalized(pInterface->Agent_GetInfo().LinearVelocity));
}
