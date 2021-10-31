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

// Fourth order Adams-Bashforth-Moulton Predictor Corrector. Fixed step version (Must reinitialize if dt is changed).
namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct ABM4prop : public Propagator<value_t>
      {
         void operator()(State &state, const value_t dt) override
         {
            auto &x = *state.x;
            auto &xd = *state.xd;
            if (state.memory.size() < 6) {
               state.memory.resize(6);
            }
            auto &x0 = state.memory[0];
            auto &xd0 = state.memory[1];
            auto &xp = state.memory[2]; // Can be used for error estimates
            auto &xd_1 = state.memory[3];
            auto &xd_2 = state.memory[4];
            auto &xd_3 = state.memory[5];

            switch (Propagator<value_t>::pass)
            {
            case 0:
               x0 = x;
               xd0 = xd;
               x = x0 + c0 * dt * (55.0 * xd0 - 59.0 * xd_1 + 37.0 * xd_2 - 9.0 * xd_3);
               xp = x;
               break;
            case 1:
               x = x0 + c0 * dt * (9.0 * xd + 19.0 * xd0 - 5.0 * xd_1 + xd_2);
               xd_3 = xd_2;
               xd_2 = xd_1;
               xd_1 = x0;
               break;
            }
         }

      private:
         static constexpr auto c0 = cx(1.0 / 24.0);
      };

      template <class value_t>
      struct ABM4stepper : public TimeStepper<value_t>
      {
         void operator()(const size_t pass, value_t &t, const value_t dt) override
         {
            switch (pass)
            {
            case 0:
               t += dt;
               break;
            case 1:
               break;
            }
         }
      };

      template <typename value_t, typename init_integrator = RK4<value_t>>
      struct ABM4
      {
         asc::Module *run_first{};

         ABM4prop<value_t> propagator;
         ABM4stepper<value_t> stepper;

         template <typename modules_t>
         void operator()(modules_t &blocks, value_t &t, const value_t dt)
         {
            if (initialized < 3)
            {
               if (run_first)
               {
                  initializer.run_first = run_first;
               }

               // TODO: This is a bit is a bit weird and inefficient 
               // state.xd may not be the derivitive at the current time after we run the initializer
               // We need the derivitive at the current time so we call update once so we can grab xd before calling the initializer
               update(blocks, run_first);
               apply(blocks);

               // Since running the initializer might overide the states we need to store them somewhere else until we have all the values
               // TODO: This assumes the container is ordered
               if (xd_temp.size() < blocks.size()) {
                  xd_temp.resize(blocks.size());
               }
               if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
               {
                  // Assign previous time step's derivative
                  size_t i = 0;
                  for (auto &block : blocks)
                  {   
                     auto &temp = xd_temp[i];
                     if (temp.size() < block.second->states.size()) {
                        temp.resize(block.second->states.size());
                     }
                     size_t j = 0;
                     for (auto &state : block.second->states)
                     {
                        temp[j][2 - initialized] = *state.xd;
                        ++j;
                     }
                     ++i;
                  }
               }
               else
               {
                  // Assign previous time step's derivative
                  size_t i = 0;
                  for (auto &block : blocks)
                  {   
                     auto &temp = xd_temp[i];
                     if (temp.size() < block->states.size()) {
                        temp.resize(block->states.size());
                     }
                     size_t j = 0;
                     for (auto &state : block->states)
                     {
                        temp[j][2 - initialized] = *state.xd;
                        ++j;
                     }
                     ++i;
                  }
               }

               // Run initializer integrator
               initializer(blocks, t, dt);

               ++initialized;

               if (initialized == 3) {
                  // Copy over stored history into state
                  // TODO: This assumes the container is ordered
                  if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
                  {
                     // Assign previous time step's derivative
                     size_t i = 0;
                     for (auto &block : blocks)
                     {
                        auto &block_store = xd_temp[i];
                        size_t j = 0;
                        for (auto &state : block.second->states)
                        {
                           auto &state_store = block_store[j];
                           if (state.memory.size() < 6) {
                              state.memory.resize(6);
                           }
                           for (size_t k = 0; k < state_store.size(); ++k) {
                              state.memory[k + 3] = state_store[k];
                           }
                           ++j;
                        }
                        ++i;
                     }
                  }
                  else
                  {
                     // Assign previous time step's derivative
                     size_t i = 0;
                     for (auto &block : blocks)
                     {
                        auto &block_store = xd_temp[i];
                        size_t j = 0;
                        for (auto &state : block->states)
                        {
                           auto &state_store = block_store[j];
                           if (state.memory.size() < 6) {
                              state.memory.resize(6);
                           }
                           for (size_t k = 0; k < state_store.size(); ++k) {
                              state.memory[k + 3] = state_store[k];
                           }
                           ++j;
                        }
                        ++i;
                     }
                  }
               }
               return;
            }

            auto &pass = propagator.pass;
            pass = 0;

            update(blocks, run_first);
            apply(blocks);
            propagate(blocks, propagator, dt);
            stepper(pass, t, dt);
            postprop(blocks);
            ++pass;

            update(blocks, run_first);
            apply(blocks);
            propagate(blocks, propagator, dt);
            stepper(pass, t, dt);
            postprop(blocks);
         }

         size_t initialized = 0;
         init_integrator initializer;
         std::vector<std::vector<std::array<value_t, 3>>> xd_temp; // Temp storage for initializer values;
      };
   }
}