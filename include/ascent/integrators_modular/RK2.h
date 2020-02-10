// Copyright (c) 2016-2017 Anyar, Inc.
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

//#include "ascent/Utility.h"
#include "ascent/modular/Module.h"
#include "ascent/integrators_modular/ModularIntegrators.h"

// Second order, two pass Runge Kutta.

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct RK2prop : public Propagator<value_t>
      {
         void operator()(State& state, const value_t dt) override
         {
            auto& x = *state.x;
            auto& xd = *state.xd;
            if (state.memory.size() < 1)
            {
               state.memory.resize(1);
            }
            auto& x0 = state.memory[0];

            switch (Propagator<value_t>::pass)
            {
            case 0:
               x0 = x;
               x = x0 + 0.5 * dt * xd;
               break;
            case 1:
               x = x0 + dt * xd;
               break;
            default:
               break;
            }
         }
      };

      template <class value_t>
      struct RK2stepper : public TimeStepper<value_t>
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

      template <typename value_t>
      struct RK2
      {
         asc::Module* run_first{};

         RK2prop<value_t> propagator;
         RK2stepper<value_t> stepper;

         template <typename modules_t>
         void operator()(modules_t& blocks, value_t& t, const value_t dt)
         {
            auto& pass = propagator.pass;
            pass = 0;

            update(blocks);
            propagate(blocks, propagator, dt);
            stepper(pass, t, dt);
            postprop(blocks);
            ++pass;

            update(blocks);
            propagate(blocks, propagator, dt);
            stepper(pass, t, dt);
            postprop(blocks);
         }
      };
   }
}