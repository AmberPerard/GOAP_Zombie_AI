# Zombie Ai with GOAP

We had the interesting task of choosing a research topic for a course in DAE. We had a AI related topic. I choose GOAP or Goal Oriented Action Planning. This is an implementation of Jeff Orkin AI architecture. He used it for F.E.A.R.

My implementation is on a 2D zombie survival game written by the DAE teachers. The project itself is written in c++.

## What is GOAP?

---

Goal Oriented Action Planning is a type of AI that uses a goal and executes actions based on a virtual state of the environment it is in. It is commonly used to allow AI to make decisions and take actions based on their goals and the available resources to them. 

GOAP works by defining a set of goals for the AI, as well as actions that the Ai can take. The Ai then then uses a planning algorithm to determine the most efficient sequence of actions to achieve its goals, taking into account the current state of the environment and the resources available to it.

For example, if an AI has no food in its inventory anymore, the sequence of actions can look something like this: “look for food”, “pick up the food” , “Eat the food. With the planning algorithm it also determines the best sequence of these actions to achieve the goal, taking into account the environmental conditions.  These can be the location of the food, distance and for instance energy of the AI.

GOAP can be useful in a variety of applications, including video games, simulations, and robotics, where it can be used to give AI characters the ability to make decisions and take actions in a realistic and dynamic environment.

## Difference with other decision making structures

---

Other well known decision making structures are Finite State Machine (FSM) or Behavior Trees (BT). 

FSM is based on a set of predefined states and transitions between those states. NPCs are in one state at a time and can only transition to other states based on specific conditions. This approach is simpler and more efficient, but can be less flexible and less capable of handling complex or dynamic behaviors.

Behaviour Trees is also based on a set of predefined action but in a more tree-like structure. Behaviour Tree allows for complex decision making by combining simple decisions in a tree-like structure. They are more flexible than FSM but still can become very chaotic and hard to manage at a certain point. 

A complex FSM can look like this:

![[https://hillside.net/plop/plop2003/Papers/Adamczyk-State-Machine.pdf](https://hillside.net/plop/plop2003/Papers/Adamczyk-State-Machine.pdf)](Untitled%207bc6a388da1643aa87ba79b0dbb6289b/Untitled.png)

[https://hillside.net/plop/plop2003/Papers/Adamczyk-State-Machine.pdf](https://hillside.net/plop/plop2003/Papers/Adamczyk-State-Machine.pdf)

A complex behavior tree can look like this: 

![[https://en.wikipedia.org/wiki/Behavior_tree](https://en.wikipedia.org/wiki/Behavior_tree)](Untitled%207bc6a388da1643aa87ba79b0dbb6289b/Untitled%201.png)

[https://en.wikipedia.org/wiki/Behavior_tree](https://en.wikipedia.org/wiki/Behavior_tree)

GOAP makes the diagrams way less complex, the one I made for my AI looks something like this

—insert image—

In summary, GOAP is a planning-based approach that allows for more complex and dynamic behavior, while FSMs and Behaviour Trees are based on predefined states and transitions, which makes them simpler and more efficient, but less flexible.
