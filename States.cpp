#include "stdafx.h"
#include "States.h"

#pragma region SuperState
SuperState::SuperState(FSMState* startState, Blackboard* pBlackboard, bool alwaysUseStartState)
	: FSMState()
	, m_pFSM{ new FiniteStateMachine(startState, pBlackboard) }
	, m_pEntryPoint{ startState }
	, m_alwaysUseStartState{ alwaysUseStartState }
{
}

SuperState::~SuperState()
{
	SAFE_DELETE(m_pFSM);
}

void SuperState::OnEnter(Blackboard* pBlackboard)
{
	if (m_alwaysUseStartState)
		m_pFSM->ForceSetState(m_pEntryPoint);
	else
		m_pEntryPoint->OnEnter(pBlackboard);
}

void SuperState::OnExit(Blackboard* pBlackboard)
{
	m_pFSM->OnExit();
}

void SuperState::Update(Blackboard* pBlackboard, float deltaTime)
{
	if (m_pFSM)
		m_pFSM->Update(deltaTime);
}

void SuperState::AddTransition(FSMState* startState, FSMState* toState, FSMTransition* transition)
{
	m_pFSM->AddTransition(startState, toState, transition);
}
#pragma endregion

void WanderState::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	Vector2 target{};
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	pBlackboard->GetData("target", target);
	if (!pAgent || !pInterface) return;

	if (DistanceSquared(pInterface->Agent_GetInfo().Position, target) < 20.f * 20.f)
	{
		const Vector2 worldDim = pInterface->World_GetInfo().Dimensions;
		Vector2 newTarget = { randomFloat(worldDim.x), randomFloat(worldDim.y) };
		newTarget -= worldDim / 2;
		// gets a random point outside of the house he currently is in
		if (pInterface->Agent_GetInfo().IsInHouse)
		{
			newTarget = { pInterface->Agent_GetInfo().Position + pInterface->Agent_GetInfo().LinearVelocity * 5.f };
			pBlackboard->ChangeData("target", pInterface->NavMesh_GetClosestPathPoint(newTarget));
			pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(newTarget));
		}

		// code to find house 
		Vector2 navTarget = {};
		for (int i = 0;; ++i)
		{
			newTarget = { randomFloat(worldDim.x), randomFloat(worldDim.y) };
			newTarget -= worldDim / 2;
			navTarget = pInterface->NavMesh_GetClosestPathPoint(newTarget);

			if (newTarget != navTarget)
				break;

			continue;
		}

		pBlackboard->ChangeData("target", pInterface->NavMesh_GetClosestPathPoint(newTarget));
		pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(newTarget));
	}
}

void WanderState::Update(Blackboard* pBlackboard, float deltaTime)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	Vector2 target{};
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("target", target)) return;
	if (!pAgent || !pInterface) return;

	pInterface->Draw_Segment(pInterface->Agent_GetInfo().Position, target, { 1.f, 0.f, 0.f });

	if (abs(DistanceSquared(target, pInterface->Agent_GetInfo().Position)) < 15.f)
		OnEnter(pBlackboard);

	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(target));
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
	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(vEntities[0].Location));

	EntityInfo entityInfo;
	for (int i = 0;; ++i)
	{
		// if no entity is found, break
		if (!pInterface->Fov_GetEntityByIndex(i, entityInfo))
			break;

		// if entity is not an item go to next entity
		if (entityInfo.Type != eEntityType::ITEM)
			continue;

		ItemInfo itemInfo{};
		pInterface->Item_GetInfo(entityInfo, itemInfo);
		if (itemInfo.Type == eItemType::GARBAGE)
			continue;
		if (itemInfo.Type == eItemType::MEDKIT)
			if (pInterface->Inventory_GetItem(2, itemInfo))
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
		return (DistanceSquared(agentPos, a.Location) < DistanceSquared(agentPos, b.Location));
	});
	pBlackboard->ChangeData("vItems", vEntities);

	// check if its in grab range
	const float grabRange{ pInterface->Agent_GetInfo().GrabRange };
	if (DistanceSquared(vEntities[0].Location, agentPos) > (grabRange * grabRange))
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
				const int idx { weaponAmmo1 < weaponAmmo2 ? 0 : 1 };
				pInterface->Inventory_RemoveItem(idx);
				if (pInterface->Inventory_AddItem(idx, pickedUpItem)) return;			
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
			if (pInterface->Inventory_AddItem(4, pickedUpItem)) return;

			const int newFoodVal{ pInterface->Food_GetEnergy(pickedUpItem) };
			ItemInfo food{};
			// food 1 ammo (no need for an if statement since we alrady return if there are no pistols in the slots yet)
			pInterface->Inventory_GetItem(3, food);
			const int foodVal1 = pInterface->Food_GetEnergy(food);
			// food 2 ammo
			pInterface->Inventory_GetItem(4, food);
			const int foodVal2 = pInterface->Food_GetEnergy(food);

			// checks if found pistol has more ammo then the current ones
			if (newFoodVal > foodVal1 || newFoodVal > foodVal2)
			{
				// switch lowest ammo first
				const int idx{ foodVal1 < foodVal2 ? 3 : 4 };
				pInterface->Inventory_UseItem(idx);
				pInterface->Inventory_RemoveItem(idx);
				if (pInterface->Inventory_AddItem(idx, pickedUpItem)) return;
			}
		}
	}
}

void EvadeState::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;

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


	static const float distCheckSqr{ 8.f * 8.f };
	const float angle{ pInterface->Agent_GetInfo().FOV_Angle / 2.f };
	const Vector2 agentPos = pInterface->Agent_GetInfo().Position;
	// calculate the three points to check if theyre safe
	const Vector2 dPointMiddle = GetNormalized(pInterface->Agent_GetInfo().LinearVelocity) * pInterface->Agent_GetInfo().FOV_Range;
	const Vector2 dPointLeft = Vector2{ dPointMiddle.x * cos(-angle) - dPointMiddle.y * sin(-angle)	, dPointMiddle.x * sin(-angle) + dPointMiddle.y * cos(-angle) };
	const Vector2 dPointRight = Vector2{ dPointMiddle.x * cos(angle) - dPointMiddle.y * sin(angle)	, dPointMiddle.x * sin(angle) + dPointMiddle.y * cos(angle) };

	// check which is a safe route
	bool isLeftSafe = true, isMiddleSafe = true, isRightSafe = true;
	for (auto& enemy : enemyVec)
	{
		if (isLeftSafe && DistanceSquared(enemy.Location, agentPos + dPointLeft) < distCheckSqr)
			isLeftSafe = false;
		if (isMiddleSafe && DistanceSquared(enemy.Location, agentPos + dPointMiddle) < distCheckSqr)
			isMiddleSafe = false;
		if (isRightSafe && DistanceSquared(enemy.Location, agentPos + dPointRight) < distCheckSqr)
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

	pBlackboard->ChangeData("target", target);
	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(target));
}

void Eat::OnEnter(Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	int slot{};
	if (!pBlackboard->GetData("interface", pInterface)) return;
	pBlackboard->GetData("eatslot", slot);

	pInterface->Inventory_UseItem(slot);
	pInterface->Inventory_RemoveItem(slot);
}

void KillFollowers::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;

	pAgent->CanRun(true);

	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(pInterface->Agent_GetInfo().Position + pInterface->Agent_GetInfo().LinearVelocity));
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
	if (canShoot > 0.999f)
	{
		ItemInfo pistol{};
		// pistol 1 ammo
		int weaponAmmo1{ 0 };
		if (pInterface->Inventory_GetItem(0, pistol))
		{
			weaponAmmo1 = pInterface->Weapon_GetAmmo(pistol);
			if (weaponAmmo1 == 0)
				pInterface->Inventory_RemoveItem(0);
		}
		// pistol 2 ammo
		int weaponAmmo2{ 0 };
		if (pInterface->Inventory_GetItem(1, pistol))
		{
			weaponAmmo2 = pInterface->Weapon_GetAmmo(pistol);
			if (weaponAmmo2 == 0)
				pInterface->Inventory_RemoveItem(1);
		}

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

	pBlackboard->ChangeData("target", target);
	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint( target ));
}

