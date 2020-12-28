#include "stdafx.h"
#include "States.h"

void WanderState::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pAgent) return;

	pAgent->SetToWander();
}

void ReturnToMap::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pAgent || !pInterface) return;

	pAgent->SetToSeek(pInterface->World_GetInfo().Center);
}

void EnterHouse::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	if (!pBlackboard->GetData("agent", pAgent))	return;
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
	EntityInfo entityInfo{};
	if (!pBlackboard->GetData("agent", pAgent))	return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("entity", entityInfo)) return;

	if (!pAgent || !pInterface) return;

	pAgent->SetToSeek(entityInfo.Location);
}

void PickUpItem::Update(Blackboard* pBlackboard, float deltaTime)
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

void EvadeState::OnEnter(Blackboard* pBlackboard)
{
	SteeringAgent* pAgent = nullptr;
	IExamInterface* pInterface = nullptr;
	vector<EnemyInfo> enemyVec{};
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("enemyVec", enemyVec)) return;
	if (!pAgent || !pInterface) return;

	// reset timer
	pAgent->ResetTimer();
	pAgent->CanRun(true);


	Vector2 target{};
	if (enemyVec.size() == 1)
	{
		Vector2 delta = enemyVec[0].Location - pInterface->Agent_GetInfo().Position;
		target = pInterface->Agent_GetInfo().Position + (Vector2{ -delta.y, delta.x } * 5.f);
	}
	else if (enemyVec.size() == 2)
	{
		if (Distance(enemyVec[0].Location, enemyVec[1].Location) > 5.f)
			target = enemyVec[0].Location + enemyVec[1].Location;
	}
	else
	{
		for (EnemyInfo& i : enemyVec)
			target += i.Location;
		
		target /= enemyVec.size();

		target = target + (pInterface->Agent_GetInfo().Position - target) * 2;
	}
	pBlackboard->ChangeData("target", target);

	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(target));
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
	Vector2 target{};
	if (!pBlackboard->GetData("agent", pAgent)) return;
	if (!pBlackboard->GetData("interface", pInterface)) return;
	if (!pBlackboard->GetData("target", target)) return;
	if (!pAgent || !pInterface) return;

	std::cout << pAgent->GetTimer() << "\n";

	pInterface->Draw_Segment(pInterface->Agent_GetInfo().Position, target, { 1.f, 0.f, 0.f });

	pAgent->SetToSeek(pInterface->NavMesh_GetClosestPathPoint(target));
}
