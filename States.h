/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/

#include "EFiniteStateMachine.h"
#include "SteeringAgent.h"
#include "EBlackboard.h"

using namespace Elite;

//------------------------------------------------
// States
//------------------------------------------------
class WanderState final : public FSMState
{
public:
	WanderState() : FSMState() {};
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class EvadeState final : public FSMState
{
public:
	EvadeState() : FSMState() {};
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void OnExit(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class ReturnToMap final : public FSMState
{
public:
	ReturnToMap() : FSMState() {};
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class EnterHouse final : public FSMState
{
public:
	EnterHouse() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void OnExit(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class EscapeHouse final : public FSMState
{
public:
	EscapeHouse() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class PickUpItem final : public FSMState
{
public:
	PickUpItem() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class Eat final : public FSMState
{
public:
	Eat() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
};

class KillFollowers final : public FSMState
{
public:
	KillFollowers() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void OnExit(Blackboard* pBlackboard) override;
};

class Heal final : public FSMState
{
public:
	Heal() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
};

class Shoot final : public FSMState
{
public:
	Shoot() : FSMState() {}
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class EscapePurge final : public FSMState
{
public:
	EscapePurge() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void OnExit(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class ShootOrEvade final : public FSMState
{
public:
	ShootOrEvade() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
};