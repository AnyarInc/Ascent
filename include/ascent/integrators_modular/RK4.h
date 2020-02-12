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

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct RK4prop : public Propagator<value_t>
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
               x = x0 + 0.5 * dt * xd0;
               break;
            case 1:
               xd1 = xd;
               x = x0 + 0.5 * dt * xd1;
               break;
            case 2:
               xd2 = xd;
               x = x0 + dt * xd2;
               break;
            case 3:
               xd3 = xd;
               x = x0 + dt / 6.0 * (xd0 + 2 * xd1 + 2 * xd2 + xd3);
               break;
            }
         }
      };

      template <class value_t>
      struct RK4stepper : public TimeStepper<value_t>
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
            case 2:
               t = t0 + dt;
               break;
            default:
               break;
            }
         }
      };

      template <class value_t>
      struct RK4
      {
         asc::Module* run_first{};

         RK4prop<value_t> propagator;
         RK4stepper<value_t> stepper;

         template <class modules_t>
         void operator()(modules_t& blocks, value_t& t, const value_t dt)
         {
            auto& pass = propagator.pass;
            pass = 0;
            
            update(blocks, run_first);
            propagate(blocks, propagator, dt);
            stepper(pass, t, dt);
            postprop(blocks);
            ++pass;

            update(blocks, run_first);
            propagate(blocks, propagator, dt);
            postprop(blocks);
            ++pass;

            update(blocks, run_first);
            propagate(blocks, propagator, dt);
            stepper(pass, t, dt);
            postprop(blocks);
            ++pass;

            update(blocks, run_first);
            propagate(blocks, propagator, dt);
            postprop(blocks);
         }
      };
   }
}
