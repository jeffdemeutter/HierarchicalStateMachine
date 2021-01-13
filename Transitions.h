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
class HouseInFov final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class ItemInFov final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class EnemyInFov final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class PickedUpAll final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class NoEnemyFovTimer final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
private:
	const float m_Timer = 1.5f;
};

class NoEnemyInFov final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class VisitedHouse final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class IsInHouse final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class IsNotInHouse final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class OutsideMap final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class InsideMap final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class Timer final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class HasToEat final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class HasToHeal final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class Play1Frame final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override { return true; };
};

class WasHit final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class HasNoBullets final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class CanKill final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class InPurgeZone final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class NotInPurge final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

class CannotKill final : public FSMTransition
{
public:
	virtual bool ToTransition(Blackboard* pBlackboard) const override;
};

#endif