# Research Topic - Hierarchical State machines (HSM/ HFSM)

## What is the problem to solve?

The biggest problem when it comes to FSM's is that they require a lot of transistions compared to states. You would need around the amount of states times itself (states * states). You already need a ton of different states, imagine how many transitions you would need. This phenomenon is called "state-explosion". This is a lot of work, since you probably also need a load of states aswell. When you make a graph of an FSM you might notice some transtitions that come from multiple states.

For instance: you have a Wanderstate, CrouchState, Jumpstate, etc... but in all these states you still want to be able to go to a Shootstate. In a normal FSM you would need to link all these seperate states towards one other state. Some of these might even need different types of transitions.
In a relatively small AI/Project, this might not really be a problem, but once you start increasing the size of the FSM it might become really confusing, unmanagable and unreadable. Even for medium projects this can get unmanagable fast.


## How do we solve this?

On paper it's fairly easy, you even might have thought about it already. If you were to make a FSM chart on paper/computer, you might have seen some states you could group up because they kind of belong together. Then you could draw/make a transition from that group to a new state. Congratulations, you just made a part of a Hierarchical (Finite) State Machine (HSM/HFSM). In HSM's these groups are called superstates and they contain multiple subStates. You could even go further and make superstates inside of superstates. Using HSM's will greatly reduce the amount of transitions you need, making it easier to read and understand. 


## How can we implement them

While doing some research about HSM's, I've found 2 ways of doing it:
- using inheritence
- making a seperate FSM in a state

I chose to implement the second one, since this is easier and more managable to implement. If I were to implement the first one, I would have to rewrite the whole FSM we already had. Also rewriting it so it would work with inheritence means it'll be dirtier code and less readable afterwards. 

So while researching and seeing diagrams of HSM's, simply said it's just an FSM in an FSM. So once you have an FSM (which we did) it's pretty simple to implement. So we just make a new state as usual, which inherits from the BaseState (In this case FSMState) and call this new state the SuperState. 

### Making a SuperState Class
This SuperState contains a new FiniteStateMachine, stores the entryState, also a boolean is passed on in the constructor of the superstate. This boolean indicates if it should always start from its startState you gave, or continue its last state he was in, in the superState. We also overload the usual State methods (OnEnter, OnExit, Update). 

#### OnEnter 
is used to start with the right subState. If the bool is set to true, it will call ForceSetState to set the startState of the FSM. If it is false then it will just call the OnEnter method of the previous active state in that SuperState
 ```c++
 void SuperState::OnEnter(Blackboard* pBlackboard)
 {
 	if (m_alwaysUseStartState)
 		m_pFSM->ForceSetState(m_pEntryPoint);
 	else
 		m_pEntryPoint->OnEnter(pBlackboard);
 }
 ```
 
 #### OnExit 
 calls the OnExit of the current subState.
```c++
 void SuperState::OnExit(Blackboard* pBlackboard)
 {
 	m_pFSM->OnExit();
 }
```
 
 #### Update 
 calls the update method of the FSM.
 ```c++
 void SuperState::Update(Blackboard* pBlackboard, float deltaTime)
 {
	 if (m_pFSM)
		 m_pFSM->Update(deltaTime);
 }
 ```
 
#### AddTransition
We create an extra method called AddTransition with fromState, toState and transition as parameters. This might look familiar to the AddTransition method of the FSM itself, because it essentially is. 
```c++
void SuperState::AddTransition(FSMState* startState, FSMState* toState, FSMTransition* transition)
{
	m_pFSM->AddTransition(startState, toState, transition);
}
```
 
### Making a SuperState Class
We will have to alter the FSM class too, since it won't work out of the box. As you might see, in the SuperState OnEnter method we call 1 of 2 methods in the FSM. One of those exists, the other one I created to make sure the SuperState starts with it's EntryState. 
#### ForceSetState Method 
just sets the called the subState from a SuperState that is passed on.
```c++
void Elite::FiniteStateMachine::ForceSetState(FSMState* pState)
{
    m_pCurrentState = pState;
    m_pCurrentState->OnEnter(m_pBlackboard);
}
```

### Result
Now we are able to create a SuperState, or even inherit from it and make a new SuperState Class with certain member variables to add in extra functionality if you'd like. 

### Reference:
  - https://web.stanford.edu/class/cs123/lectures/CS123_lec08_HFSM_BT.pdf
  - https://barrgroup.com/embedded-systems/how-to/introduction-hierarchical-state-machines
  - https://towardsdatascience.com/hierarchical-finite-state-machine-for-ai-acting-engine-9b24efc66f2
