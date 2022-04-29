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
   namespace direct
   {
      /// Fourth order, four pass Runge Kutta integrator.
      template <class value_t>
      struct RK4
      {
         template <class system_t, class states_t>
         void operator()(system_t&& system, states_t& states, value_t& t, const value_t dt)
         {
            const auto t0 = t;
            const auto dt_2 = 0.5_v*dt;
            const auto dt_6 = cx(1.0 / 6.0)*dt;

            const size_t n = states.size();
            if (x0.size() < n)
            {
               x0.resize(n);
               xd.resize(n);
            }

            size_t i{};
            for (; i < n; ++i)
               x0[i] = *states[i].x;

            system();
            for (i = 0; i < n; ++i)
            {
               xd[i] = *states[i].xd;
               *states[i].x = dt_2 * xd[i] + x0[i];
            }
            t += dt_2;

            system();
            for (i = 0; i < n; ++i)
            {
               const auto& xdi = *states[i].xd;
               xd[i] += 2 * xdi;
               *states[i].x = dt_2 * xdi + x0[i];
            }

            system();
            for (i = 0; i < n; ++i)
            {
               const auto& xdi = *states[i].xd;
               xd[i] += 2 * xdi;
               *states[i].x = dt * xdi + x0[i];
            }
            t = t0 + dt;

            system();
            for (i = 0; i < n; ++i)
            {
               *states[i].x = dt_6 * (xd[i] + *states[i].xd) + x0[i];
            }
         }

         std::vector<value_t> x0, xd;
      };
   }
}