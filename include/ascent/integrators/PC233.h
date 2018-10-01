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
#include "ascent/integrators/RK2.h"

// P-2/PC-3/C-3 algorithm, which has the same error coefficient and order as the P-3/PC-3/C-3 predictor-corrector, but is more stable

// Real Time (RT) Adam's Moulton predictor-corrector integration
// Source: R.M. Howe. A new family of real-time predictor-corrector integration algorithms. The University of Michigan. September 1991.

namespace asc
{
   template <typename state_t, typename init_integrator = RK4T<state_t>>
   struct PC233T
   {
      using value_t = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, const value_t dt)
      {
         if (!initialized)
         {
            initializer(system, x, t, dt);
            xd_1 = initializer.xd;
            initialized = true;
            return;
         }

         const value_t t0 = t;
         const value_t dt_3 = cx(1.0 / 3.0)*dt;

         const size_t n = x.size();
         if (xd0.size() < n)
         {
            xd0.resize(n);
            xd_temp.resize(n);
         }

         x0 = x;
         system(x0, xd0, t);
         size_t i{};
         for (; i < n; ++i)
            x[i] = x0[i] + c0 * dt * (7.0*xd0[i] - xd_1[i]); // X(n + 1/3), third step computation
         t += dt_3;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + c1 * dt * (39.0*xd_temp[i] - 4.0*xd0[i] + xd_1[i]); // X(n + 2/3), two thirds step computation
         t += dt_3;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + c2 * dt * (xd0[i] + 3.0*xd_temp[i]);
         xd_1 = xd0;
         t = t0 + dt;
      }

   private:
      bool initialized{};
      init_integrator initializer;
      state_t x0, xd0, xd_temp;
      state_t xd_1; // -1, previous time step derivative

      static constexpr auto c0 = cx(1.0 / 18.0);
      static constexpr auto c1 = cx(1.0 / 54.0);
      static constexpr auto c2 = cx(1.0 / 4.0);
   };
}