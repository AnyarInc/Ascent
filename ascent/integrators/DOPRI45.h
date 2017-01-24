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

// Runge Kutta Dormand Prince 45

namespace asc
{
   template <typename state_t>
   struct DOPRI45T
   {
      using value_t = typename state_t::value_type;

      // epsilon is the error tolerance
      DOPRI45T(const value_t abs_tol, const value_t rel_tol) : abs_tol(abs_tol), abs_tol_64(abs_tol / static_cast<value_t>(64.0)),
         rel_tol(rel_tol), rel_tol_64(rel_tol / static_cast<value_t>(64.0)) {}

      bool adaptive{};

      bool initialized{};

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, value_t& dt)
      {
         const value_t t0 = t;
         const double dt_5 = r_1_5 * dt;

         const size_t n = x.size();
         if (xd0.size() < n)
         {
            xd0.resize(n);
            xd1.resize(n);
            xd2.resize(n);
            xd3.resize(n);
            xd4.resize(n);
            xd5.resize(n);
         }

         x0 = x;
         
         if (!initialized)
         {
            system(x, xd0, t);
            initialized = true;
         }

         size_t i{};
         for (; i < n; ++i)
            x[i] = x0[i] + dt_5 * xd0[i];
         t += dt_5;

         system(x, xd1, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c10 * xd0[i] + c11 * xd1[i]);
         t = t0 + r_3_10 * dt;

         system(x, xd2, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c20 * xd0[i] + c21 * xd1[i] + c22 * xd2[i]);
         t = t0 + r_4_5 * dt;

         system(x, xd3, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c30 * xd0[i] + c31 * xd1[i] + c32 * xd2[i] + c33 * xd3[i]);
         t = t0 + r_8_9 * dt;

         system(x, xd4, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c40 * xd0[i] + c41 * xd1[i] + c42 * xd2[i] + c43 * xd3[i] + c44 * xd4[i]);
         t = t0 + dt;

         system(x, xd5, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c50 * xd0[i] + c52 * xd2[i] + c53 * xd3[i] + c54 * xd4[i] + c55 * xd5[i]);

         if (adaptive)
         {
            for (i = 0; i < n; ++i)
               xd2[i] = e0 * xd0[i] + e2 * xd2[i] + e3 * xd3[i] + e4 * xd4[i] + e5 * xd5[i]; // reuse xd2 memory to free up xd0 so that we can assign to it

            system(x, xd0, t); // xd6 is xd0, because first same as last (FSAL)

            // see https://en.wikipedia.org/wiki/Adaptive_stepsize

            value_t x5th, x4th;
            value_t abs_diff, rel_diff;
            value_t max_abs_diff{};
            value_t max_rel_diff{};
            for (i = 0; i < n; ++i)
            {
               // 4th order estimate
               x4th = x0[i] + dt * (xd2[i] + e6 * xd0[i]);
               x5th = x[i];
               abs_diff = abs(x4th - x5th); // absolute error estimate

               if (abs_diff > max_abs_diff)
                  max_abs_diff = abs_diff;

               if (abs(x4th) <= abs(x5th))
                  rel_diff = abs_diff / abs(x4th);
               else
                  rel_diff = abs_diff / abs(x5th);

               if (rel_diff > max_rel_diff)
                  max_rel_diff = rel_diff;
            }

            const value_t R = r_1_5 * max_abs_diff;
            const value_t E = r_1_5 * max_rel_diff;

            if (R > abs_tol || E > rel_tol)
            {
               dt *= half; // reduce the time step
               t = t0;
               x = x0;
               operator()(system, x, t, dt); // recompute the solution recursively
            }
            else if (R <= abs_tol_64 && E <= rel_tol_64)
               dt *= 1.1; // increase the time step for future steps
         }
         else
            system(x, xd0, t);
      }

   private:
      static constexpr auto half = static_cast<value_t>(0.5);

      static constexpr auto r_1_5 = static_cast<value_t>(1.0 / 5.0); // also c00
      static constexpr auto r_3_10 = static_cast<value_t>(3.0 / 10.0);
      static constexpr auto r_4_5 = static_cast<value_t>(4.0 / 5.0);
      static constexpr auto r_8_9 = static_cast<value_t>(8.0 / 9.0);

      static constexpr auto c10 = static_cast<value_t>(3.0 / 40.0);
      static constexpr auto c11 = static_cast<value_t>(9.0 / 40.0);

      static constexpr auto c20 = static_cast<value_t>(44.0 / 45.0);
      static constexpr auto c21 = static_cast<value_t>(-56.0 / 15.0);
      static constexpr auto c22 = static_cast<value_t>(32.0 / 9.0);

      static constexpr auto c30 = static_cast<value_t>(19372.0 / 6561.0);
      static constexpr auto c31 = static_cast<value_t>(-25360.0 / 2187.0);
      static constexpr auto c32 = static_cast<value_t>(64448.0 / 6561.0);
      static constexpr auto c33 = static_cast<value_t>(-212.0 / 729.0);

      static constexpr auto c40 = static_cast<value_t>(9017.0 / 3168.0);
      static constexpr auto c41 = static_cast<value_t>(-355.0 / 33.0);
      static constexpr auto c42 = static_cast<value_t>(46732.0 / 5247.0);
      static constexpr auto c43 = static_cast<value_t>(49.0 / 176.0);
      static constexpr auto c44 = static_cast<value_t>(-5103.0 / 18656.0);

      static constexpr auto c50 = static_cast<value_t>(35.0 / 384.0);
      // c51 is 0
      static constexpr auto c52 = static_cast<value_t>(500.0 / 1113.0);
      static constexpr auto c53 = static_cast<value_t>(125.0 / 192.0);
      static constexpr auto c54 = static_cast<value_t>(-2187.0 / 6784.0);
      static constexpr auto c55 = static_cast<value_t>(11.0 / 84.0);

      static constexpr auto e0 = static_cast<value_t>(5179.0 / 57600.0);
      // e1 is 0
      static constexpr auto e2 = static_cast<value_t>(7571.0 / 16695.0);
      static constexpr auto e3 = static_cast<value_t>(393.0 / 640.0);
      static constexpr auto e4 = static_cast<value_t>(-92097.0 / 339200.0);
      static constexpr auto e5 = static_cast<value_t>(187.0 / 2100.0);
      static constexpr auto e6 = static_cast<value_t>(1.0 / 40.0);

      state_t x0, xd0, xd1, xd2, xd3, xd4, xd5;
      const value_t abs_tol, abs_tol_64, rel_tol, rel_tol_64;
   };
}