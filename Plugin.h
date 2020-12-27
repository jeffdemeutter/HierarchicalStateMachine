#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"

#include "SteeringAgent.h"
#include "StatesAndTransitions.h"
#include "EFiniteStateMachine.h"

class IBaseInterface;
class IExamInterface;

class Plugin :public IExamPlugin
{
public:
	Plugin() {};
	virtual ~Plugin() {};

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

private:
	//Interface, used to request data from/perform actions with the AI Framework
	IExamInterface* m_pInterface = nullptr;
	vector<HouseInfo> GetHousesInFOV() const;
	vector<EntityInfo> GetEntitiesInFOV() const;

	SteeringAgent* m_pSteeringAgent = nullptr;

	std::vector<Elite::FSMState*> m_pStates{};
	std::vector<Elite::FSMTransition*> m_pTransitions{};

	// demo stuff
	Elite::Vector2 m_Target = {};
};


//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}