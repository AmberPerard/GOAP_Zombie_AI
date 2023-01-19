# Zombie Ai with GOAP

We had the interesting task of choosing a research topic for a course in DAE. We had a AI related topic. I choose GOAP or Goal Oriented Action Planning. This is an implementation of Jeff Orkin AI architecture. He used it for F.E.A.R.

My implementation is on a 2D zombie survival game written by the DAE teachers. The project itself is written in c++.

## What is GOAP?

Goal Oriented Action Planning is a type of AI that uses a goal and executes actions based on a virtual state of the environment it is in. It is commonly used to allow AI to make decisions and take actions based on their goals and the available resources to them. 

GOAP works by defining a set of goals for the AI, as well as actions that the Ai can take. The Ai then then uses a planning algorithm to determine the most efficient sequence of actions to achieve its goals, taking into account the current state of the environment and the resources available to it.

For example, if an AI has no food in its inventory anymore, the sequence of actions can look something like this: “look for food”, “pick up the food” , “Eat the food. With the planning algorithm it also determines the best sequence of these actions to achieve the goal, taking into account the environmental conditions.  These can be the location of the food, distance and for instance energy of the AI.

GOAP can be useful in a variety of applications, including video games, simulations, and robotics, where it can be used to give AI characters the ability to make decisions and take actions in a realistic and dynamic environment.

## Difference with other decision making structures

Other well known decision making structures are Finite State Machine (FSM) or Behavior Trees (BT). 

FSM is based on a set of predefined states and transitions between those states. NPCs are in one state at a time and can only transition to other states based on specific conditions. This approach is simpler and more efficient, but can be less flexible and less capable of handling complex or dynamic behaviors.

Behaviour Trees is also based on a set of predefined action but in a more tree-like structure. Behaviour Tree allows for complex decision making by combining simple decisions in a tree-like structure. They are more flexible than FSM but still can become very chaotic and hard to manage at a certain point. 

A complex FSM can look like this:

-instert image-

[https://hillside.net/plop/plop2003/Papers/Adamczyk-State-Machine.pdf](https://hillside.net/plop/plop2003/Papers/Adamczyk-State-Machine.pdf)

A complex behavior tree can look like this: 

-insert image-

[https://en.wikipedia.org/wiki/Behavior_tree](https://en.wikipedia.org/wiki/Behavior_tree)

GOAP makes the diagrams way less complex, the one I made for my AI looks something like this

—insert image—

In summary, GOAP is a planning-based approach that allows for more complex and dynamic behavior, while FSMs and Behaviour Trees are based on predefined states and transitions, which makes them simpler and more efficient, but less flexible.

## Creation of my GOAP AI

### Actions

most fundamental thing for a decision making structure are the actions that are defined for the AI to execute. A GOAP give the AI a set of **Preconditions** of the world and a set of ****************Effects**************** that it will affect the state of the world. 

The example below shows the a medkit consuming action. For this action to be able to execute there are 2 preconditions. The AI needs to have a medkit in its inventory and the AI needs to have lost some health.

```cpp
GOAP::Action_ConsumeMedKit::Action_ConsumeMedKit()
	:BaseGoapAction("ConsumeMedKit", 2)
{
	BaseGoapAction::SetPrecondition("medkitInInv", true);
	BaseGoapAction::SetPrecondition("LowHealth", true);
	BaseGoapAction::SetEffect("LowHealth", false);
	BaseGoapAction::SetEffect("medkitInInv", false);
}
```

After the action is executed the effects on the world state will be that the AI doesn’t have a medkit anymore and it will not longer be low health. 

I have create a base action that all actions can inherit from. This will give the action a name and a set cost. This cost will become useful in the planner. 

### World State

The world states represents how the AI perceives the world. It is a set of Booleans that indicate certain conditions in the world. These what gives the preconditions to the actions.

```cpp
("insidePurgezone", false);
("inDanger", false);
("LowHealth", false);
("LowFood", false);

("enemiesInRange", false);
("houseInRange", false);
("targetInRange", false);

("pistolInInv", false);
("foodInInv", false);
("shotgunInInv", false);
("medkitInInv", false);
```

### Goals

With the world state Booleans initialized, we can set certain goals for the desired world that we want the AI to be in. 

In the example down below you can see that we have a goal to shoot the enemies if the AI is in danger. It also has a priority number with it. 

```cpp
struct Goal_ShootEnemies final : WorldState
{
public:
	Goal_ShootEnemies() : WorldState("ShootEnemies", 400)
	{
		SetCondition("inDanger", false);
	}
}
```

in contrary to the looting house goal the priority for shooting enemies is way higher.

```cpp
Goal_LootHouse() : WorldState("LootHouse", 100)
```

This is important for our AI so that the planner can make the right decisions on what is the most important at that current moment. 

### Planner

The planner is what makes the AI so flexible, It is the brain that puts together the path of actions that it will take for the highest available priority goal. The Planning algorithm I use for the Planner is A*. This Algorithm is well know for finding the shortest path on a nav mesh, but it can generally be use to find a shortest path if you have cost in your nodes. 

—continue later—
