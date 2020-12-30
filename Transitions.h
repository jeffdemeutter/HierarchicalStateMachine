/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "EFiniteStateMachine.h"
#include "SteeringAgent.h"
#include "EBlackboard.h"

using namespace Elite;

//---------------------------------------------------------------------------------------------------
// Transitions
//---------------------------------------------------------------------------------------------------
class HouseInFov : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class ItemInFov : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class EnemyInFov : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class PickedUpAll : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class NoEnemyInFov : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class VisitedHouse : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class IsInVisitedHouse : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class IsNotInHouse : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class OutsideMap : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class InsideMap : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class Timer : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class HasToEat : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class HasToHeal : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class Play1Frame : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override { return true; };
};

class WasHit : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class GotKill : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class HasNoBullets : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class HasBullets : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class LowStamina : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

#endif