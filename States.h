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

class EnterHouse : public FSMState
{
public:
	EnterHouse() : FSMState() {}
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class EscapeHouse : public FSMState
{
public:
	EscapeHouse() : FSMState() {}
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class PickUpItem : public FSMState
{
public:
	PickUpItem() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
};

class Eat : public FSMState
{
public:
	Eat() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
};

class RunAway : public FSMState
{
public:
	RunAway() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
	virtual void OnExit(Blackboard* pBlackboard) override;
};

class Heal : public FSMState
{
public:
	Heal() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
};

class Shoot : public FSMState
{
public:
	Shoot() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
	virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
private:
	float m_Timer;
};