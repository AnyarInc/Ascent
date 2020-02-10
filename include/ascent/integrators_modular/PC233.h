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

#include "ascent/Utility.h"
#include "ascent/integrators_modular/ModularIntegrators.h"
#include "ascent/integrators_modular/RK4.h"

// P-2/PC-3/C-3 algorithm, which has the same error coefficient and order as the P-3/PC-3/C-3 predictor-corrector, but is more stable

// Real Time (RT) Adam's Moulton predictor-corrector integration
// Source: R.M. Howe. A new family of real-time predictor-corrector integration algorithms. The University of Michigan. September 1991.

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct PC233prop : public Propagator<value_t>
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
            auto& xd_1 = state.memory[4];

            switch (Propagator<value_t>::pass)
            {
            case 0:
               x0 = x;
               xd0 = xd;
               x = x0 + c0 * dt * (7 * xd0 - xd_1);   // X(n + 1/3), third step computation
               break;
            case 1:
               xd1 = xd;
               x = x0 + c1 * dt * (39 * xd1 - 4 * xd0 + xd_1);   // X(n + 2/3), two thirds step computation
               break;
            case 2:
               xd2 = xd;
               x = x0 + c2 * dt * (xd0 + 3 * xd2);
               xd_1 = xd0;
               break;
            }
         }

      private:
         static constexpr auto c0 = cx(1.0 / 18.0);
         static constexpr auto c1 = cx(1.0 / 54.0);
         static constexpr auto c2 = cx(1.0 / 4.0);
      };

      template <class value_t>
      struct PC233stepper : public TimeStepper<value_t>
      {
         value_t t0{};

         void operator()(const size_t pass, value_t& t, const value_t dt) override
         {
            switch (pass)
            {
            case 0:
               t0 = t;
               t += (1.0 / 3.0) * dt;
               break;
            case 1:
               t += (1.0 / 3.0) * dt;
               break;
            case 2:
               t = t0 + dt;
               break;
            }
         }
      };

      template <typename value_t, typename init_integrator = RK4<value_t>>
      struct PC233
      {
         asc::Module* run_first{};

         PC233prop<value_t> propagator;
         PC233stepper<value_t> stepper;

         template <typename modules_t>
         void operator()(modules_t& blocks, value_t& t, const value_t dt)
         {
            if (!initialized)
            {
               if (run_first)
               {
                  initializer.run_first = run_first;
               }
               // Run initializer integrator
               initializer(blocks, t, dt);

               if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
               {
                  // Assign previous time step's derivative
                  for (auto& block : blocks)
                  {
                     for (auto& state : block.second->states)
                     {
                        state.memory.resize(5);
                        auto& xd_1 = state.memory[4];
                        xd_1 = *state.xd;
                     }
                  }
               }
               else
               {
                  // Assign previous time step's derivative
                  for (auto& block : blocks)
                  {
                     for (auto& state : block->states)
                     {
                        state.memory.resize(5);
                        auto& xd_1 = state.memory[4];
                        xd_1 = *state.xd;
                     }
                  }
               }

               initialized = true;
               return;
            }
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
            ++pass;

            update(blocks);
            propagate(blocks, propagator, dt);
            stepper(pass, t, dt);
            postprop(blocks);
         }

      private:
         bool initialized{};
         init_integrator initializer;
      };
   }
   
}