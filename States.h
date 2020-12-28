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
private:
	Vector2 delta{};
};

class ReturnToMap final : public FSMState
{
public:
	ReturnToMap() : FSMState() {};
	virtual void OnEnter(Blackboard* pBlackboard) override;
};

class EnterHouse : public FSMState
{
public:
	EnterHouse() : FSMState() {}
	virtual void OnEnter(Blackboard* pBlackboard) override;
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
