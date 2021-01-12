#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"


using namespace Elite;

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "Bot-Tboske";
	info.Student_FirstName = "Jeff";
	info.Student_LastName = "De Meutter";
	info.Student_Class = "2DAE01";

	// initialize
	m_pSteeringAgent = new SteeringAgent();

	// ===================================================================
	// decisionmaking init
	Blackboard* pBlackboard = new Blackboard();
	pBlackboard->AddData("interface", m_pInterface);
	pBlackboard->AddData("agent"	, m_pSteeringAgent);
	pBlackboard->AddData("target"	, m_Target);
	pBlackboard->AddData("house"	, HouseInfo{});
	pBlackboard->AddData("entity"	, EntityInfo{});
	pBlackboard->AddData("enemy"	, EnemyInfo{});
	pBlackboard->AddData("vItems"	, deque<EntityInfo>{});
	pBlackboard->AddData("enemyVec"	, vector<EnemyInfo>{});
	pBlackboard->AddData("purge"	, PurgeZoneInfo{});
	pBlackboard->AddData("timer"	, static_cast<float>(0.f));
	pBlackboard->AddData("kills"	, static_cast<int>(0));
	pBlackboard->AddData("distance"	, 5.f);

	// ===================================================================
	// Create states
	EnterHouse*		pEnterHouse		= new EnterHouse();
	WanderState*	pWanderState	= new WanderState();
	PickUpItem*		pPickupItem		= new PickUpItem();
	ReturnToMap*	pReturnToMap	= new ReturnToMap();
	EvadeState*		pEvadeState		= new EvadeState();
	Eat*			pEat			= new Eat();
	KillFollowers*	pKillFollowers	= new KillFollowers();
	Heal*			pHeal			= new Heal();
	Shoot*			pShoot			= new Shoot();
	ShootOrEvade*	pShootOrEvade	= new ShootOrEvade();
	EscapePurge*	pEscapePurge	= new EscapePurge();

	// Super States
	SuperState*		pGather			= new SuperState(pWanderState, pBlackboard, false);
	SuperState*		pCombat			= new SuperState(pShootOrEvade, pBlackboard, true);

	SuperState*		pDefault		= new SuperState(pGather, pBlackboard, false);
	
	m_pStates.push_back(pEnterHouse);
	m_pStates.push_back(pWanderState);
	m_pStates.push_back(pPickupItem);
	m_pStates.push_back(pReturnToMap);
	m_pStates.push_back(pEvadeState);
	m_pStates.push_back(pEat);
	m_pStates.push_back(pKillFollowers);
	m_pStates.push_back(pHeal);
	m_pStates.push_back(pShoot);
	m_pStates.push_back(pShootOrEvade);
	m_pStates.push_back(pEscapePurge);

	m_pStates.push_back(pGather);
	m_pStates.push_back(pCombat);
	m_pStates.push_back(pDefault);

	// ===================================================================
	// Create transitions
	HouseInFov*			pHouseInFov			= new HouseInFov();
	ItemInFov*			pItemInFov			= new ItemInFov();
	PickedUpAll*		pPickedUpAll		= new PickedUpAll();
	VisitedHouse*		pVisitedHouse		= new VisitedHouse();
	OutsideMap*			pOutsideMap			= new OutsideMap();
	InsideMap*			pInsideMap			= new InsideMap();
	EnemyInFov*			pEnemyInFov			= new EnemyInFov();
	NoEnemyFovTimer*	pNoEnemyFovTimer	= new NoEnemyFovTimer();
	NoEnemyInFov*		pNoEnemyInFov		= new NoEnemyInFov();
	Timer*				pTimer				= new Timer();
	Timer*				pTimerSOE			= new Timer();
	Play1Frame*			pPlay1Frame			= new Play1Frame();
	HasToEat*			pHasToEat			= new HasToEat();
	WasHit*				pWasHit				= new WasHit();
	HasToHeal*			pHasToHeal			= new HasToHeal();
	HasNoBullets*		pHasNoBullets		= new HasNoBullets();
	CanKillEnemies*		pCanKillEnemies		= new CanKillEnemies();
	InPurgeZone*		pInPurgeZone		= new InPurgeZone();
	NotInPurge*			pNotInPurge			= new NotInPurge();
	CannotKill*			pCannotKill			= new CannotKill();

	m_pTransitions.push_back(pHouseInFov);
	m_pTransitions.push_back(pItemInFov);
	m_pTransitions.push_back(pPickedUpAll);
	m_pTransitions.push_back(pVisitedHouse);
	m_pTransitions.push_back(pOutsideMap);
	m_pTransitions.push_back(pInsideMap);
	m_pTransitions.push_back(pEnemyInFov);
	m_pTransitions.push_back(pNoEnemyFovTimer);
	m_pTransitions.push_back(pNoEnemyInFov);
	m_pTransitions.push_back(pTimer);
	m_pTransitions.push_back(pPlay1Frame);
	m_pTransitions.push_back(pHasToEat);
	m_pTransitions.push_back(pWasHit);
	m_pTransitions.push_back(pHasToHeal);
	m_pTransitions.push_back(pHasNoBullets);
	m_pTransitions.push_back(pCanKillEnemies);
	m_pTransitions.push_back(pInPurgeZone);
	m_pTransitions.push_back(pNotInPurge);
	m_pTransitions.push_back(pCannotKill);


	// ===================================================================
	// Super State Init
		// Wander <-> ReturnToMap
		pGather->AddTransition(pWanderState	, pReturnToMap	, pOutsideMap		);
		pGather->AddTransition(pReturnToMap	, pWanderState	, pInsideMap		);
		// ReturnToMap -> PickupItem
		pGather->AddTransition(pReturnToMap	, pPickupItem	, pItemInFov		);
		// Wander <-> PickupItem
		pGather->AddTransition(pPickupItem	, pWanderState	, pPickedUpAll		);
		pGather->AddTransition(pWanderState	, pPickupItem	, pItemInFov		);
		// Wander <-> EnterHouse
		pGather->AddTransition(pWanderState	, pEnterHouse	, pHouseInFov		);
		pGather->AddTransition(pEnterHouse	, pWanderState	, pItemInFov		);
		pGather->AddTransition(pEnterHouse	, pWanderState	, pVisitedHouse		);


		// ShootOrEvade -> EvadeState
		pCombat->AddTransition(pShootOrEvade, pEvadeState	, pCannotKill		);
		// Shoot -> EvadeState
		pCombat->AddTransition(pShoot		, pEvadeState	, pCannotKill		);
		pCombat->AddTransition(pShoot		, pShootOrEvade	, pNoEnemyInFov		);
		// ShootOrEvade -> Shoot
		pCombat->AddTransition(pShootOrEvade, pShoot		, pCanKillEnemies	);


		//// Gather <-> Combat
		//pDefault->AddTransition(pCombat		, pGather		, pNoEnemyInFov);
		//pDefault->AddTransition(pGather		, pCombat		, pEnemyInFov);


	// ===================================================================
	// FSM
	FiniteStateMachine* pFSM = new FiniteStateMachine(pGather, pBlackboard);
		// Gather <-> Combat
		pFSM->AddTransition(pCombat			, pGather		, pNoEnemyInFov	);
		pFSM->AddTransition(pGather			, pCombat		, pEnemyInFov	);
		// Gather -> KillFollowers -> Combat
		pFSM->AddTransition(pGather			, pKillFollowers, pWasHit		);
		pFSM->AddTransition(pKillFollowers	, pCombat		, pEnemyInFov	);
		// Gather <-> EscapePurge
		pFSM->AddTransition(pGather			, pEscapePurge	, pInPurgeZone	);
		pFSM->AddTransition(pEscapePurge	, pGather		, pNotInPurge	);
		// Combat -> EscapePurge
		pFSM->AddTransition(pCombat			, pEscapePurge	, pInPurgeZone	);


	m_pSteeringAgent->SetDecisionMaking(pFSM);

	//FiniteStateMachine* pFSM = new FiniteStateMachine(pWanderState, pBlackboard);


	// Apply Transitions
	//pFSM->AddTransition(pWanderState	, pEnterHouse	, pHouseInFov		);

	////		to Wander
	//pFSM->AddTransition(pKillFollowers	, pWanderState	, pTimer			);
	//									  
	//pFSM->AddTransition(pShoot			, pWanderState	, pNoEnemyInFov		);
	//pFSM->AddTransition(pShootOrEvade	, pWanderState	, pTimerSOE			);
	//pFSM->AddTransition(pEvadeState		, pWanderState	, pTimer			);
	//									  
	//pFSM->AddTransition(pReturnToMap	, pWanderState	, pInsideMap		);
	//pFSM->AddTransition(pEscapePurge	, pWanderState	, pNotInPurge		);
	//pFSM->AddTransition(pEnterHouse		, pWanderState	, pVisitedHouse		);
	//pFSM->AddTransition(pPickupItem		, pWanderState	, pPickedUpAll		);



	////		pickup stuff
	//pFSM->AddTransition(pWanderState	, pPickupItem	, pItemInFov		);
	//pFSM->AddTransition(pReturnToMap	, pPickupItem	, pItemInFov		);
	//pFSM->AddTransition(pEnterHouse		, pPickupItem	, pItemInFov		);

	//pFSM->AddTransition(pWanderState	, pReturnToMap	, pOutsideMap		);

	////		choose from evade or kill
	//pFSM->AddTransition(pWanderState	, pShootOrEvade	, pEnemyInFov		);
	//pFSM->AddTransition(pReturnToMap	, pShootOrEvade	, pEnemyInFov		);
	//pFSM->AddTransition(pPickupItem		, pShootOrEvade	, pEnemyInFov		);
	//pFSM->AddTransition(pKillFollowers	, pShootOrEvade	, pEnemyInFov		);

	//pFSM->AddTransition(pWanderState	, pKillFollowers, pWasHit			);
	//pFSM->AddTransition(pReturnToMap	, pKillFollowers, pWasHit			);
	////		shoot
	//pFSM->AddTransition(pShootOrEvade	, pShoot		, pCanKillEnemies	);
	////		evade
	//pFSM->AddTransition(pShootOrEvade	, pEvadeState	, pCannotKill		);
	//
	////		purge zone stuff
	//pFSM->AddTransition(pWanderState	, pEscapePurge	, pInPurgeZone		);
	//pFSM->AddTransition(pReturnToMap	, pEscapePurge	, pInPurgeZone		);
	//pFSM->AddTransition(pEnterHouse		, pEscapePurge	, pInPurgeZone		);
	//pFSM->AddTransition(pShootOrEvade	, pEscapePurge	, pInPurgeZone		);

	////		Using Utilities
	//pFSM->AddTransition(pWanderState	, pEat			, pHasToEat			);
	//pFSM->AddTransition(pWanderState	, pHeal			, pHasToHeal		);

	//pFSM->AddTransition(pEat			, pWanderState	, pPlay1Frame		);
	//pFSM->AddTransition(pHeal			, pWanderState	, pPlay1Frame		);



	// set fsm
	//m_pSteeringAgent->SetDecisionMaking(pFSM);
}

//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded
};

//Called only once
void Plugin::DllShutdown()
{
	//Called wheb the plugin gets unloaded

}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be usefull to inspect certain behaviours (Default = false)
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	//auto agentInfo = m_pInterface->Agent_GetInfo();

	//auto nextTargetPos = m_Target; //To start you can use the mouse position as guidance

	//auto vHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	//auto vEntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)

	//for (auto& e : vEntitiesInFOV)
	//{
	//	if (e.Type == eEntityType::PURGEZONE)
	//	{
	//		PurgeZoneInfo zoneInfo;
	//		m_pInterface->PurgeZone_GetInfo(e, zoneInfo);
	//		std::cout << "Purge Zone in FOV:" << e.Location.x << ", "<< e.Location.y <<  " ---EntityHash: " << e.EntityHash << "---Radius: "<< zoneInfo.Radius << std::endl;
	//	}
	//}

	return m_pSteeringAgent->Update(dt, m_pInterface->Agent_GetInfo());
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}