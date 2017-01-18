# Ascent

An extremely fast and flexible C++ simulation engine and differential equation solver.

Built from the ground up as a major advancement of [Ascent-beta](https://github.com/AnyarInc/ascent-beta)

## Blazingly Fast

Ascent outperforms boost's [odeint](http://headmyshoulder.github.io/odeint-v2/index.html)

Integration algorithms include automatical vectorization for std::vector and std::array

## Extremely Flexible

Ascent solvers conform to the odeint system syntax, letting you run odeint system without changes (and with faster results!)

Ascent allows the user to solve complex, dynamic systems of differential equations in a modular, object-oriented manner.

Easily multi-thread systems, swap systems, and swap integrators on the fly.

## Highlights
- Header Only
- Automatic Vectorization: Ascent conforms to vectorization standards (such as Intel's) and is vectorized by default in MSVC and Xcode
- Free for open source and commercial applications (Apache License)
- Modular: solve systems in an object-oriented manner
- Variable Tracking: Optimized recording of variable time history
- Asynchronous Sampling and Event Scheduling
- Integrators
	- Euler
	- Runge Kutta (2nd, 4th)
	- Runge Kutta Merson's Method (with adaptive stepping)
    - Dormand Prince (45, 87, with adaptive stepping)
    - Multiple real-time predictor-correctors
- Scripting Interface: Optional ChaiScript interface to easily script simulations

## Applications
- Aerospace, multi-body physics, chemical reactions, economics, and much more
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
[Ascent-beta](https://github.com/AnyarInc/ascent-beta) still remains a state of the art simulation engine, far surpassing the capabilities of many modern engines/ode solvers.

However, this new version of Ascent takes the flexibility of Ascent-beta to another level while providing (as far as we know) the best performance in the world.

## How This New Ascent Is Better
- Header only
- Much, much faster. With a simple spring-mass-damper simulation, the current version is twelve-times faster than the beta version.
- Cleaner code: Ascent doesn't require as many specialized containers, avoid pointers, and takes advantage of more core C++. This makes it faster to write simulations, the code is more comprehensible, and it is easier to debug.
- State-space modeling: Ascent allows state space modeling and allows it to be integrated with modular design.
- Module/Simulation abstraction: Modules are only simulation specific if they directly handle integration states. This means modules can easily be used across simulations even while running.
- Easier, faster, and more powerful scripting: Simulation loops can now be scripted in Ascent. Scripts are also easier to move to C++ if the user wants to compile simulation designs.
- More straightforward multi-threading
- This new version completely separates concepts of simulators, integrators, systems, modules, recorders, and more.
- Integration algorithms have reduced memory footprints (RK4 uses less than half the memory as before)


## Alternatives
### [odeint](https://github.com/boostorg/odeint)
odeint is a C++ ordinary differential equation solver that is part of the boost library.  
Ascent was partly inspired by the design of odeint, but improves upon it in speed and flexibility.  
Ascent is capable of solving the same state-space systems as odeint (systems can be directly copied).  
However, Ascent is also designed to solve object-oriented, modular simulations, which odeint cannot handle because of internal state handling design choices.  
odeint still offers various solvers that Ascent currently does not support, so it is certainly worth checking out!
