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

// Five pass Runge Kutta Merson's Method. With adaptive time stepping.

namespace asc
{
   template <typename state_t>
   struct RKMMT
   {
      using value_t = typename state_t::value_type;

      // epsilon is the error tolerance
      RKMMT(const value_t epsilon) : epsilon(epsilon), epsilon_64(epsilon / static_cast<value_t>(64.0)) {}

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, value_t& dt)
      {
         const value_t t0 = t;
         const value_t dt_2 = 0.5_v*dt;
         const value_t dt_3 = cx(1.0 / 3.0)*dt;
         const value_t dt_6 = cx(1.0 / 6.0)*dt;
         const value_t dt_8 = cx(1.0 / 8.0)*dt;

         const size_t n = x.size();
         if (xd0.size() < n)
         {
            xd0.resize(n);
            xd2_temp.resize(n);
            xd3_temp.resize(n);
            xd_temp.resize(n);
            x3_temp.resize(n);
         }

         x0 = x;
         system(x0, xd0, t);
         size_t i{};
         for (; i < n; ++i)
            x[i] = dt_3 * xd0[i] + x0[i];
         t += dt_3;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = dt_6 * (xd0[i] + xd_temp[i]) + x0[i];

         system(x, xd2_temp, t);
         for (i = 0; i < n; ++i)
         {
            xd2_temp[i] *= 3; // all uses of xd2 are 3*xd2, so we perform this multiplication only once
            x[i] = dt_8 * (xd0[i] + xd2_temp[i]) + x0[i];
         }
         t = t0 + dt_2;

         system(x, xd3_temp, t);
         for (i = 0; i < n; ++i)
         {
            xd3_temp[i] *= 4; // all uses of xd3 are 4*xd3, so we perform this multiplication only once
            x[i] = dt_2 * (xd0[i] - xd2_temp[i] + xd3_temp[i]) + x0[i];
         }
         x3_temp = x; // used for error estimation
         t = t0 + dt;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = dt_6 * (xd0[i] + xd3_temp[i] + xd_temp[i]) + x0[i];

         // Adaptive time stepping would probably be best handled by a constexpr if, because we want the handling to be determined at compile time, perhaps by a templated boolean
         // https://www.encyclopediaofmath.org/index.php/Kutta-Merson_method#Eq-2

         value_t abs_diff;
         value_t max_abs_diff{};
         for (i = 0; i < n; ++i)
         {
            abs_diff = abs(x3_temp[i] - x[i]); // absolute error estimate

            if (abs_diff > max_abs_diff)
               max_abs_diff = abs_diff;
         }

         const value_t R = fifth * max_abs_diff;

         if (R > epsilon)
         {
            dt *= half; // reduce the time step
            t = t0;
            x = x0;
            operator()(system, x, t, dt); // recompute the solution recursively
         }
         else if (R <= epsilon_64)
            dt *= 2; // increase the time step for future steps
      }

   private:
      state_t x0, xd0, xd2_temp, xd3_temp, xd_temp;
      state_t x3_temp;
      const value_t epsilon, epsilon_64;
   };
}