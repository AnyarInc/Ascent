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

#include "ascent/Utility.h"

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct RK4
      {
         template <class modules_t>
         void operator()(modules_t& modules, value_t& t, const value_t dt)
         {
            pass = 0;
            const auto t0 = t;

            update(modules);
            propagate(modules, dt);
            t += 0.5 * dt;

            update(modules);
            propagate(modules, dt);

            update(modules);
            propagate(modules, dt);
            t = t0 + dt;

            update(modules);
            propagate(modules, dt);
         }

      private:
         size_t pass{};

         void propagate(State& state, const double dt)
         {
            auto& x = *state.x;
            auto& xd = *state.xd;
            state.memory.resize(5);
            auto& x0 = state.memory[0];
            auto& xd0 = state.memory[1];
            auto& xd1 = state.memory[2];
            auto& xd2 = state.memory[3];
            auto& xd3 = state.memory[4];

            switch (pass)
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

         template <class modules_t>
         void propagate(modules_t& modules, const value_t dt)
         {
            for (auto& module : modules)
            {
               for (auto& state : module->states)
               {
                  propagate(state, dt);
               }
            }
            ++pass;
         }

         template <class modules_t>
         void update(modules_t& modules)
         {
            for (auto& module : modules)
            {
               (*module)();
            }
         }
      };
   }
}