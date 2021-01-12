# ZombieSurvivalAI

The aim of this project is to create an artificial brain for an agent that traverses a world invested by zombies

- Look for items
- Aim and shoot zombies
- Try and survive as long as possible


# Research Topic - Hierarchical State machines (HSM/ HFSM)

**What is the problem to solve?**

The biggest problem when it comes to FSM's is that they require a lot of transistions compared to states. U would need around the amount of states times itself (states * states). You already need a ton of different states, imagine how many transitions you would need. This phenomenon is called "state-explosion". This is a lot of work, since you probably also need a load of states aswell. When you make a graph of an FSM you might notice some transtitions that come from multiple states.

For instance: you have a Wanderstate, CrouchState, Jumpstate, etc... but in all these states you still want to be able to go to a Shootstate. In a normal FSM you would need to link all these seperate states towards one other state. 
In a relatively small AI/Project, this might not really be a problem, but once you start increasing the size of the FSM it might become really confusing, unmanagable and unreadable.


**How do we solve this?**

On paper it's fairly easy, you might have even thought about it yourself. If you were to make a schematic on paper, you might have grouped up some states, and just drawn 1 transition towards it. Congratulations, this is basicly the same way a Hierarchical State Machine (HSM) works. In HSM's these groups are called superstates and they contain multiple states. You could even go a little bit further and make superstates inside of superstates. Using HSM's will greatly reduce the amount of transitions you need, making it easier to read and understand.


**How can we implement them**

While doing some research about HSM's, i've found you could do this with inheritence. 




**Reference:**
  - https://web.stanford.edu/class/cs123/lectures/CS123_lec08_HFSM_BT.pdf
  - https://barrgroup.com/embedded-systems/how-to/introduction-hierarchical-state-machines
  - https://towardsdatascience.com/hierarchical-finite-state-machine-for-ai-acting-engine-9b24efc66f2
