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

// Runge Kutta Dormand Prince 45

namespace asc
{
   template <typename state_t>
   struct DOPRI45T
   {
      using value_t = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, const value_t dt)
      {
         const auto t0 = t;
         const auto dt_5 = 0.2_v * dt;

         const auto n = x.size();
         if (xd0.size() < n)
         {
            xd0.resize(n);
            xd_temp.resize(n);
            xd2.resize(n);
            xd3.resize(n);
            xd4.resize(n);
         }

         x0 = x;

         if (!fsal_computed) // if an adaptive stepper hasn't computed the first same as last state, we must compute the step here
         {
            system(x0, xd0, t);
            fsal_computed = false;
         }

         size_t i{};
         for (; i < n; ++i)
            x[i] = x0[i] + dt_5 * xd0[i];
         t += dt_5;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c10 * xd0[i] + c11 * xd_temp[i]);
         t = t0 + cx(3.0 / 10.0) * dt;

         system(x, xd2, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c20 * xd0[i] + c21 * xd_temp[i] + c22 * xd2[i]);
         t = t0 + cx(4.0 / 5.0) * dt;

         system(x, xd3, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c30 * xd0[i] + c31 * xd_temp[i] + c32 * xd2[i] + c33 * xd3[i]);
         t = t0 + cx(8.0 / 9.0) * dt;

         system(x, xd4, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c40 * xd0[i] + c41 * xd_temp[i] + c42 * xd2[i] + c43 * xd3[i] + c44 * xd4[i]);
         t = t0 + dt;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c50 * xd0[i] + c52 * xd2[i] + c53 * xd3[i] + c54 * xd4[i] + c55 * xd_temp[i]);
      }

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, value_t& dt, const AdaptiveT<value_t>& settings)
      {
         const value_t abs_tol = settings.abs_tol;
         const value_t rel_tol = settings.rel_tol;
         const value_t safety_factor = settings.safety_factor;

         const value_t t0 = t;
         const size_t n = x.size();

         xd6.resize(n);

start_adaptive:
         operator()(system, x, t, dt);

         system(x, xd6, t); // xd6 is xd0, because first same as last (FSAL)

         // overwrite xd2 as the error estimate, this lets us vectorize the calculation of errors and saves memory
         for (size_t i = 0; i < n; ++i)
         {
            xd2[i] = abs(x0[i] + dt * (e0 * xd0[i] + e2 * xd2[i] + e3 * xd3[i] + e4 * xd4[i] + e5 * xd_temp[i] + e6 * xd6[i]) - x[i]); // absolute error estimate (x4th - x5th)
         }

         value_t e, e_max{};
         for (size_t i = 0; i < n; ++i)
         {
            //e = xd2[i] / (abs_tol + rel_tol * (1.0 * abs(x0[i]) + 0.01 * abs(xd0[i])));
            e = xd2[i] / (abs_tol + rel_tol * (abs(x0[i]) + 0.01 * abs(xd0[i])));
         
            if (e > e_max)
               e_max = e;
         }
         
         if (e_max > 1.0_v)
         {
            dt *= std::max(0.9_v * pow(e_max, -cx(1.0 / 3.0)), 0.2_v);

            t = t0;
            x = x0;

            goto start_adaptive; // recompute the solution recursively
         }

         if (e_max < 0.5_v)
         {
            e_max = std::max(3.2e-4_v, e_max); // 3.2e-4 = pow(5, -5)
            dt *= 0.9_v * pow(e_max, -0.2_v);
         }

         xd0 = xd6;
         fsal_computed = true;
      }

   private:
      bool fsal_computed = false;

      static constexpr auto c10 = cx(3.0 / 40.0);
      static constexpr auto c11 = cx(9.0 / 40.0);

      static constexpr auto c20 = cx(44.0 / 45.0);
      static constexpr auto c21 = cx(-56.0 / 15.0);
      static constexpr auto c22 = cx(32.0 / 9.0);

      static constexpr auto c30 = cx(19372.0 / 6561.0);
      static constexpr auto c31 = cx(-25360.0 / 2187.0);
      static constexpr auto c32 = cx(64448.0 / 6561.0);
      static constexpr auto c33 = cx(-212.0 / 729.0);

      static constexpr auto c40 = cx(9017.0 / 3168.0);
      static constexpr auto c41 = cx(-355.0 / 33.0);
      static constexpr auto c42 = cx(46732.0 / 5247.0);
      static constexpr auto c43 = cx(49.0 / 176.0);
      static constexpr auto c44 = cx(-5103.0 / 18656.0);

      static constexpr auto c50 = cx(35.0 / 384.0);
      // c51 is 0
      static constexpr auto c52 = cx(500.0 / 1113.0);
      static constexpr auto c53 = cx(125.0 / 192.0);
      static constexpr auto c54 = cx(-2187.0 / 6784.0);
      static constexpr auto c55 = cx(11.0 / 84.0);

      static constexpr auto e0 = cx(5179.0 / 57600.0);
      // e1 is 0
      static constexpr auto e2 = cx(7571.0 / 16695.0);
      static constexpr auto e3 = cx(393.0 / 640.0);
      static constexpr auto e4 = cx(-92097.0 / 339200.0);
      static constexpr auto e5 = cx(187.0 / 2100.0);
      static constexpr auto e6 = cx(1.0 / 40.0);

      state_t x0, xd0, xd_temp, xd2, xd3, xd4, xd6; // xd_temp is used for xd1 and xd5
   };
}