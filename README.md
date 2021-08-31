# Ascent

An extremely fast and flexible C++ simulation engine and differential equation solver.

See the [Ascent Wiki](https://github.com/AnyarInc/Ascent/wiki) for more examples and help

Study the [modular-spring-damper](https://github.com/AnyarInc/Ascent/wiki/modular-spring-damper) example to learn the basics of solving matrix free systems of ordinary differential equations in an object-oriented manner

## Blazingly Fast

Ascent's integration algorithms are designed for speed, and outperform boost's [odeint](http://headmyshoulder.github.io/odeint-v2/index.html) in both Debug and Release

<img src="https://github.com/AnyarInc/Ascent/wiki/graphics/speedup-ratio.PNG" width="400"><img src="https://github.com/AnyarInc/Ascent/wiki/graphics/Lorenz.PNG" width="300">

Integration algorithms are automatically vectorized when using std::vector

## Extremely Flexible

Ascent solvers conform to the odeint system syntax, letting you run odeint system without changes (and with faster results!)

odeint solvers can also be used to run Ascent simulations, providing all flexibility of the odeint engine with Ascent's simulation framework

Ascent can solve complex, dynamic systems of differential equations in a modular, object-oriented manner

Easily multi-thread systems and change integrators on the fly

## Highlights
- Header Only
- Automatic Vectorization: Ascent conforms to vectorization standards (such as Intel's)
- Free for open source and commercial applications (Apache License)
- Modular (Optional): solve systems in an object-oriented manner
- Variable Tracking: Optimized recording of variable time history
- Asynchronous Sampling and Event Scheduling
- Multiple Integration Algorithms (In Progress): adaptive steppers, predictor-correctors, etc.
- Use boost's odeint library as the numerical integration engine

***
## Requirements
- C++17 compliant compiler

## Applications
- Aerospace, multi-body physics, chemical reactions, economics, circuits, and much more
- As a game engine for synchronization and physics
- Agent-based simulations
- Complex systems of differential equations
- State-space modeling
- Control algorithms (e.g. robotics)

***
### About [odeint](https://github.com/boostorg/odeint)
odeint is a C++ ordinary differential equation solver that is part of the boost library.  
Ascent was partly inspired by the design of odeint, but Ascent offers better performance where comparisons can be made, this is especially true for solving object-oriented systems.  
odeint offers various state types and solvers that Ascent integration algorithms currently do not support, so the odeint solvers are a viable option as the integration algorithm beneath an Ascent simulation.
