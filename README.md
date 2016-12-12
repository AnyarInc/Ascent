# Ascent 2

A modern, open source, C++ simulation engine and differential equation solver. Designed for maximum speed and flexibility.

Built from the ground up as a major advancement of [Ascent](http://anyarinc.github.io/ascent/)

## Blazingly Fast

Ascent outperforms boost's odeint while also allowing for greater flexibility.

## Extremely Flexible

Ascent allows the user to solve complex, dynamic systems of differential equqtions in a modular, object-oriented manner.

Ascent completely separates concepts of simulators, integrators, systems, modules, recorders, and more.

Ascent makes it easy to multi-thread systems and swap out systems, integrators, and more on the fly.

## Highlights
- Header Only
- Automatic vectorization: Ascent conforms to vectorization standards (such as Intel's) and is vectorized by default in MSVC and Xcode
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
## Prerequisites
- C++14 compliant compiler

## Scripting
- [ChaiScript](http://chaiscript.com/) Embedded Scripting Language (Optional for scripting)

***
## *Why Version 2?*
Version 1 of Ascent is currently a state of the art simulation engine, far surpassing the capabilities of many modern engines/ode solvers.

Ascent 2 takes the flexibility of Ascent to another level while providing the fastest performance in the world.

## How Ascent Version 2 Is Better
- Header only
- Much faster. With a simple spring-mass-damper simulation, Ascent 2 is twelve-times faster than version 1.
- Cleaner code: Ascent 2 doesn't require as many specialed containers, doesn't use pointers, and takes advantage of more core C++. This makes it faster to write simulations, the code is more comprehensible, and it is easier to debug.
- State-space modeling: Ascent 2 allows state space modeling and allows it to be integrated with modular design. Odeint systems can also be directly solved in Ascent 2.
- Module/Simulation abstraction: Modules are only simulation specific if they directly handle integration states. This means modules can easily be used across simulations even while running.
- Easier, faster, and more powerful scripting: Scripting in Ascent 2 has almost no speed reduction compared with compiled code. Scripts are also easier to move to C++ if the user wants to compile simulation designs.
- More straightforward multi-threading


## Alternatives
### [odeint](https://github.com/boostorg/odeint)
odeint is a C++ ordinary differential equation solver that is part of the boost library.  
Ascent was partly inspired by the design of odeint, but Ascent is faster and more flexible.  
Ascent can solve the same state-space systems as odeint (systems can be directly copied).  
odeint was also not designed for modular simulations, which Ascent handles with ease.  
Ascent is also significantly faster in Debug runs.
