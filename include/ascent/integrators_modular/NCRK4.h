// Copyright (c) 2016-2019 Anyar, Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "ascent/modular/Module.h"
#include "ascent/integrators_modular/ModularIntegrators.h"

// Non-confluent RK4 method ( "3/8th's Rule" )

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct NCRK4prop : public Propagator<value_t>
      {
         void operator()(State& state, const value_t dt) override
         {
            auto& x = *state.x;
            auto& xd = *state.xd;
            state.memory.resize(5);
            auto& x0 = state.memory[0];
            auto& xd0 = state.memory[1];
            auto& xd1 = state.memory[2];
            auto& xd2 = state.memory[3];
            auto& xd3 = state.memory[4];

            switch (Propagator<value_t>::pass)
            {
            case 0:
               x0 = x;
               xd0 = xd;  // k1
               x = x0 + (1.0/3.0) * dt * xd0;
               break;
            case 1:
               xd1 = xd;  // k2
               x = x0 + dt * ( (-1.0/3.0) * xd0 +  xd1) ;
               break;
            case 2:
               xd2 = xd;  // k3
               x = x0 + dt * ( xd0 - xd1 + xd2);
               break;
            case 3:
               xd3 = xd; // k4
               x = x0 + dt / 8.0 * (xd0 + 3.0 * xd1 + 3.0 * xd2 + xd3);
               break;
            }
         }
      };

      template <class value_t>
      struct NCRK4stepper : public TimeStepper<value_t>
      {
         value_t t0{};

         void operator()(const size_t pass, value_t& t, const value_t dt) override
         {
            switch (pass)
            {
            case 0:
               t0 = t;
               t += 1.0/3.0 * dt;
               break;
            case 1:
               t = t0 + 2.0/3.0 * dt;
               break;
            case 2:
               t = t0 + dt;
               break;
            default:
               break;
            }
         }
      };

      template <class value_t>
      struct NCRK4
      {
         NCRK4prop<value_t> propagator;
         NCRK4stepper<value_t> stepper;

         template <class modules_t>
         void operator()(modules_t& modules, value_t& t, const value_t dt)
         {
            auto& pass = propagator.pass;
            pass = 0;
            
            update(modules);
            propagate(modules, dt);
            stepper(pass, t, dt);
            ++pass;

            update(modules);
            propagate(modules, dt);
            ++pass;

            update(modules);
            propagate(modules, dt);
            stepper(pass, t, dt);
            ++pass;

            update(modules);
            propagate(modules, dt);
         }

         template <class modules_t>
         void update(modules_t& modules)
         {
            for (auto& module : modules)
            {
               (*module)();
            }
         }

         template <class modules_t>
         void propagate(modules_t& modules, const value_t dt)
         {
            for (auto& module : modules)
            {
               module->propagate(propagator, dt);
            }
         }
      };
   }
}
