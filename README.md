# ZombieSurvivalAI

The aim of this project is to create an artificial brain for an agent that traverses a world invested by zombies

- Look for items
- Aim and shoot zombies
- Try and survive as long as possible


# Research Topic - Hierarchical/Nested State machines

**What is the problem to solve?**

The biggest problem when it comes to FSM's is that they require a lot of transistions compared to states. U would need around the amount of states times itself (states * states). This is a lot of work, since you probably also need a load of states aswell. When you make a graph of an FSM you might notice some transtitions that come from multiple states.

For instance: you have a Wanderstate, CrouchState, Jumpstate, etc... but in all these states you still want to be able to go to a Shootstate. In a normal FSM you would need to link all these seperate states towards one other state. In a relatively small AI/Project, this might not really be a problem, but once you start increasing the size of the FSM it might become really confusing and unreadable.


**How do we solve this?**

