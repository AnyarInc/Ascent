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

// Fourth order, two pass Real Time (RT) Adams Moulton predictor-corrector integrator
// A new family of real-time predictor-corrector integration algorithms
// R.M. Howe. A new family of real-time predictor-corrector integration algorithms. The University of Michigan. September 1991.

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct RTAM4prop : public Propagator<value_t>
      {
         void operator()(State& state, const value_t dt) override
         {
            auto& x = *state.x;
            auto& xd = *state.xd;
            if (state.memory.size() < 5)
            {
               state.memory.resize(5);
            }
            auto& x0 = state.memory[0];
            auto& xd0 = state.memory[1];
            auto& xd1 = state.memory[2];
            auto& xd2 = state.memory[3];
            auto& xd3 = state.memory[4];

            switch (Propagator<value_t>::pass)
            {
            case 0:
               x0 = x;
               xd0 = xd;
               x = x0 + dt / 384 * (297 * xd - 187 * xd1 + 107 * xd2 - 25 * xd3);
               break;
            case 1:
               x = x0 + dt / 30 * (36 * xd - 10 * xd0 + 5 * xd1 - xd2);
               xd3 = xd2;
               xd2 = xd1;
               xd1 = xd0;
               break;
            }
         }
      };

      template <class value_t>
      struct RTAM4stepper : public TimeStepper<value_t>
      {
         value_t t0{};

         void operator()(const size_t pass, value_t& t, const value_t dt) override
         {
            switch (pass)
            {
            case 0:
               t0 = t;
               t += 0.5 * dt;
               break;
            case 1:
               t = t0 + dt;
               break;
            default:
               break;
            }
         }
      };

      template <class value_t>
      struct RTAM4
      {
         asc::Timing<double>* timing{};

         RTAM4prop<value_t> propagator;
         RTAM4stepper<value_t> stepper;

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
            stepper(pass, t, dt);
         }

         template <class modules_t>
         void update(modules_t& modules)
         {
            if (timing)
            {
               (*timing)();
            }
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