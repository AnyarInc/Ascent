# Ascent

A fast and flexible C++ simulation engine and differential equation solver.

Built from the ground up as a major advancement of [Ascent-beta](https://github.com/AnyarInc/ascent-beta)

## Blazingly Fast

Ascent outperforms boost's odeint while also allowing for greater flexibility.

## Extremely Flexible

Ascent allows the user to solve complex, dynamic systems of differential equqtions in a modular, object-oriented manner.

Ascent completely separates concepts of simulators, integrators, systems, modules, recorders, and more.

Ascent makes it easy to multi-thread systems and swap out systems, integrators, and more on the fly.

## Highlights
- Header Only
- Automatic Vectorization: Ascent conforms to vectorization standards (such as Intel's) and is vectorized by default in MSVC and Xcode
- Free for open source and commercial applications (Apache License)
- Modular
- Variable Tracking: Optimized recording of variable time history
- Asynchronous Sampling and Event Scheduling
- Solve odeint systems in Ascent
- Integrators
	- Euler
	- Runge Kutta (2nd, 4th)
	- Runge Kutta Merson's Method (with daptive stepping)
    - Dormand Prince (45, 87, with adaptive stepping)
    - Multiple real-time predictor-correctors
    
## Applications
- Aerospace, multi-body physics, chemical reactions, encomonics, and much more
- As a game engine for synchronization and physics
- Agent-based simulations
- Complex systems of differential equations
- State-space modeling
- Control algorithms (e.g. robotics)

***
## Requirements
- C++14 compliant compiler

## Scripting Requirements
- [ChaiScript](http://chaiscript.com/) Embedded Scripting Language

***
## *Why A New Version of Ascent?*
Ascent-beta is still a state of the art simulation engine, far surpassing the capabilities of many modern engines/ode solvers.

However, this new version of Ascent takes the flexibility of Ascent-beta to another level while providing the fastest performance in the world.

## How This New Ascent Is Better
- Header only
- Much faster. With a simple spring-mass-damper simulation, Ascent is twelve-times faster than the beta version.
- Cleaner code: Ascent doesn't require as many specialed containers, doesn't use pointers, and takes advantage of more core C++. This makes it faster to write simulations, the code is more comprehensible, and it is easier to debug.
- State-space modeling: Ascent allows state space modeling and allows it to be integrated with modular design. Odeint systems can also be directly solved in Ascent.
- Module/Simulation abstraction: Modules are only simulation specific if they directly handle integration states. This means modules can easily be used across simulations even while running.
- Easier, faster, and more powerful scripting: Scripting in Ascent has almost no speed reduction compared with compiled code. Scripts are also easier to move to C++ if the user wants to compile simulation designs.
- More straightforward multi-threading


## Alternatives
### [odeint](https://github.com/boostorg/odeint)
odeint is a C++ ordinary differential equation solver that is part of the boost library.  
Ascent was partly inspired by the design of odeint, but Ascent is faster and more flexible.  
Ascent is capable of solving the same state-space systems as odeint (systems can be directly copied).  
However, Ascent is also designed to handle modular simulations, unlike odeint.  
Ascent is also significantly faster than odeint in Debug runs.
