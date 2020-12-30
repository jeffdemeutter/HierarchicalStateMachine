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
	pBlackboard->AddData("timer"	, static_cast<float>(0.f));
	pBlackboard->AddData("kills"	, static_cast<int>(0));
	pBlackboard->AddData("distance"	, 5.f);

	// ===================================================================
	// Create states
	EnterHouse*		pEnterHouse		= new EnterHouse();
	WanderState*	pWanderState	= new WanderState();
	PickUpItem*		pPickupItem		= new PickUpItem();
	EscapeHouse*	pEscapeHouse	= new EscapeHouse();
	ReturnToMap*	pReturnToMap	= new ReturnToMap();
	EvadeState*		pEvadeState		= new EvadeState();
	Eat*			pEat			= new Eat();
	RunAway*		pRunAway		= new RunAway();
	Heal*			pHeal			= new Heal();
	Shoot*			pShoot			= new Shoot();
	FSMState*		pShootOrEvade	= new FSMState();
	TurnAround*		pTurnAround		= new TurnAround();
	
	m_pStates.push_back(pEnterHouse);
	m_pStates.push_back(pWanderState);
	m_pStates.push_back(pPickupItem);
	m_pStates.push_back(pEscapeHouse);
	m_pStates.push_back(pReturnToMap);
	m_pStates.push_back(pEvadeState);
	m_pStates.push_back(pEat);
	m_pStates.push_back(pRunAway);
	m_pStates.push_back(pHeal);
	m_pStates.push_back(pShoot);
	m_pStates.push_back(pShootOrEvade);
	m_pStates.push_back(pTurnAround);

	// ===================================================================
	// Create transitions
	HouseInFov*			pHouseInFov			= new HouseInFov();
	ItemInFov*			pItemInFov			= new ItemInFov();
	PickedUpAll*		pPickedUpAll		= new PickedUpAll();
	VisitedHouse*		pVisitedHouse		= new VisitedHouse();
	IsInVisitedHouse*	pIsInVisitedHouse	= new IsInVisitedHouse();
	IsNotInHouse*		pIsNotInHouse		= new IsNotInHouse();
	OutsideMap*			pOutsideMap			= new OutsideMap();
	InsideMap*			pInsideMap			= new InsideMap();
	EnemyInFov*			pEnemyInFov			= new EnemyInFov();
	NoEnemyInFov*		pNoEnemyInFov		= new NoEnemyInFov();
	Timer*				pTimer				= new Timer();
	Play1Frame*			pPlay1Frame			= new Play1Frame();
	HasToEat*			pHasToEat			= new HasToEat();
	WasHit*				pWasHit				= new WasHit();
	HasToHeal*			pHasToHeal			= new HasToHeal();
	GotKill*			pGotKill			= new GotKill();
	HasNoBullets*		pHasNoBullets		= new HasNoBullets();
	HasBullets*			pHasBullets			= new HasBullets();
	LowStamina*			pLowStamina			= new LowStamina();

	m_pTransitions.push_back(pHouseInFov);
	m_pTransitions.push_back(pItemInFov);
	m_pTransitions.push_back(pPickedUpAll);
	m_pTransitions.push_back(pVisitedHouse);
	m_pTransitions.push_back(pIsInVisitedHouse);
	m_pTransitions.push_back(pIsNotInHouse);
	m_pTransitions.push_back(pOutsideMap);
	m_pTransitions.push_back(pInsideMap);
	m_pTransitions.push_back(pEnemyInFov);
	m_pTransitions.push_back(pNoEnemyInFov);
	m_pTransitions.push_back(pTimer);
	m_pTransitions.push_back(pPlay1Frame);
	m_pTransitions.push_back(pHasToEat);
	m_pTransitions.push_back(pWasHit);
	m_pTransitions.push_back(pHasToHeal);
	m_pTransitions.push_back(pHasNoBullets);
	m_pTransitions.push_back(pGotKill);
	m_pTransitions.push_back(pHasBullets);
	m_pTransitions.push_back(pLowStamina);


	// ===================================================================
	// FSM
	FiniteStateMachine* pFSM = new FiniteStateMachine(pWanderState, pBlackboard);

	// Apply Transitions
	pFSM->AddTransition(pWanderState	, pEnterHouse	, pHouseInFov		);

	//		pickup stuff
	pFSM->AddTransition(pWanderState	, pPickupItem	, pItemInFov		);
	pFSM->AddTransition(pEnterHouse		, pPickupItem	, pItemInFov		);
	pFSM->AddTransition(pEscapeHouse	, pPickupItem	, pItemInFov		);
	pFSM->AddTransition(pPickupItem		, pEscapeHouse	, pPickedUpAll		);

	pFSM->AddTransition(pEnterHouse		, pWanderState	, pVisitedHouse		);
	pFSM->AddTransition(pWanderState	, pEscapeHouse	, pIsInVisitedHouse	);
	pFSM->AddTransition(pEscapeHouse	, pWanderState	, pIsNotInHouse		);

	pFSM->AddTransition(pWanderState	, pReturnToMap	, pOutsideMap		);
	pFSM->AddTransition(pReturnToMap	, pWanderState	, pInsideMap		);


	//		choose from evade or kill
	pFSM->AddTransition(pWanderState	, pShootOrEvade	, pEnemyInFov		);
	pFSM->AddTransition(pEnterHouse		, pShootOrEvade	, pEnemyInFov		);
	pFSM->AddTransition(pReturnToMap	, pShootOrEvade	, pEnemyInFov		);
	pFSM->AddTransition(pEscapeHouse	, pShootOrEvade	, pEnemyInFov		);
	pFSM->AddTransition(pPickupItem		, pShootOrEvade	, pEnemyInFov		);
	//		shoot
	pFSM->AddTransition(pShootOrEvade	, pShoot		, pHasBullets		);
	pFSM->AddTransition(pShoot			, pWanderState	, pNoEnemyInFov		);
	pFSM->AddTransition(pShoot			, pWanderState	, pGotKill			);
	//		evade
	pFSM->AddTransition(pShootOrEvade	, pEvadeState	, pHasNoBullets		);
	pFSM->AddTransition(pEvadeState		, pWanderState	, pTimer			);


	//		Run Away
	pFSM->AddTransition(pWanderState	, pRunAway		, pWasHit			);
	pFSM->AddTransition(pReturnToMap	, pRunAway		, pWasHit			);
	pFSM->AddTransition(pEscapeHouse	, pRunAway		, pWasHit			);


	pFSM->AddTransition(pRunAway		, pTurnAround	, pLowStamina		);
	pFSM->AddTransition(pRunAway		, pWanderState	, pTimer			);

	pFSM->AddTransition(pTurnAround		, pShootOrEvade	, pEnemyInFov		);

	//		Using Utilities
	pFSM->AddTransition(pWanderState	, pEat			, pHasToEat			);
	pFSM->AddTransition(pEat			, pWanderState	, pPlay1Frame		);
	pFSM->AddTransition(pWanderState	, pHeal			, pHasToHeal		);
	pFSM->AddTransition(pHeal			, pWanderState	, pPlay1Frame		);




	// set fsm
	m_pSteeringAgent->SetDecisionMaking(pFSM);
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
	params.SpawnEnemies = false; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be usefull to inspect certain behaviours (Default = false)
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	//{
	//	m_CanRun = true;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	//{
	//	m_AngSpeed -= Elite::ToRadians(10);
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	//{
	//	m_AngSpeed += Elite::ToRadians(10);
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	//{
	//	m_GrabItem = true;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	//{
	//	m_UseItem = true;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	//{
	//	m_RemoveItem = true;
	//}
	//else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	//{
	//	m_CanRun = false;
	//}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();

	auto nextTargetPos = m_Target; //To start you can use the mouse position as guidance

	auto vHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	auto vEntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)

	for (auto& e : vEntitiesInFOV)
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			PurgeZoneInfo zoneInfo;
			m_pInterface->PurgeZone_GetInfo(e, zoneInfo);
			std::cout << "Purge Zone in FOV:" << e.Location.x << ", "<< e.Location.y <<  " ---EntityHash: " << e.EntityHash << "---Radius: "<< zoneInfo.Radius << std::endl;
		}
	}
	

	//INVENTORY USAGE DEMO
	//********************
	//if (m_GrabItem)
	//{
	//	ItemInfo item;
	//	//Item_Grab > When DebugParams.AutoGrabClosestItem is TRUE, the Item_Grab function returns the closest item in range
	//	//Keep in mind that DebugParams are only used for debugging purposes, by default this flag is FALSE
	//	//Otherwise, use GetEntitiesInFOV() to retrieve a vector of all entities in the FOV (EntityInfo)
	//	//Item_Grab gives you the ItemInfo back, based on the passed EntityHash (retrieved by GetEntitiesInFOV)
	//	if (m_pInterface->Item_Grab({}, item))
	//	{
	//		//Once grabbed, you can add it to a specific inventory slot
	//		//Slot must be empty
	//		m_pInterface->Inventory_AddItem(0, item);
	//	}
	//}

	//if (m_UseItem)
	//{
	//	//Use an item (make sure there is an item at the given inventory slot)
	//	m_pInterface->Inventory_UseItem(0);
	//}

	//if (m_RemoveItem)
	//{
	//	//Remove an item from a inventory slot
	//	m_pInterface->Inventory_RemoveItem(0);
	//}

		
	//m_pSteeringAgent->SetToRotate(m_Target);
	//return m_pSteeringAgent->Update(dt, m_pInterface->Agent_GetInfo());
	return m_pSteeringAgent->Update(dt, m_pInterface->Agent_GetInfo());
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
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