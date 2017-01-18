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

// Five pass Runge Kutta Merson's Method. With adaptive time stepping.

namespace asc
{
   namespace core
   {
      template <typename state_t>
      typename state_t::value_type maxAbsDiff(const state_t& x0, const state_t& x1)
      {
         typename state_t::value_type maximum{};
         typename state_t::value_type temp;
         const size_t n = x0.size();
         for (size_t i = 0; i < n; ++i)
         {
            temp = abs(x0[i] - x1[i]);
            if (temp > maximum)
               maximum = temp;
         }

         return maximum;
      }
   }

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
         const value_t dt_2 = half*dt;
         const value_t dt_3 = third*dt;
         const value_t dt_6 = sixth*dt;
         const value_t dt_8 = eigth*dt;

         const size_t n = x.size();
         if (xd0.size() < n)
         {
            xd0.resize(n);
            xd2.resize(n);
            xd3.resize(n);
            x_estimate.resize(n);
            xd_temp.resize(n);
         }

         x0 = x;
         system(x, xd0, t);
         for (size_t i = 0; i < n; ++i)
            x[i] = dt_3 * xd0[i] + x0[i];
         t += dt_3;

         system(x, xd_temp, t);
         for (size_t i = 0; i < n; ++i)
            x[i] = dt_6 * (xd0[i] + xd_temp[i]) + x0[i];

         system(x, xd2, t);
         for (size_t i = 0; i < n; ++i)
            x[i] = dt_8 * (xd0[i] + 3.0*xd2[i]) + x0[i];
         t = t0 + dt_2;

         system(x, xd3, t);
         for (size_t i = 0; i < n; ++i)
            x[i] = dt_2 * (xd0[i] - 3.0*xd2[i] + 4.0*xd3[i]) + x0[i];
         t = t0 + dt;

         system(x, xd_temp, t);
         for (size_t i = 0; i < n; ++i)
            x[i] = dt_6 * (xd0[i] + 4.0*xd3[i] + xd_temp[i]) + x0[i];

         // Adaptive time stepping would probably be best handled by a constexpr if, because we want the handling to be determined at compile time, perhaps by a templated boolean
         // https://www.encyclopediaofmath.org/index.php/Kutta-Merson_method#Eq-2

         for (size_t i = 0; i < n; ++i)
            x_estimate[i] = dt_2 * (xd0[i] - 3.0*xd2[i] + 4.0*xd3[i]) + x0[i];

         const value_t R = fifth * core::maxAbsDiff(x_estimate, x); // error estimate

         if (R > epsilon)
         {
            dt *= half; // reduce the time step
            t = t0;
            x = x0;
            operator()(system, x, t, dt); // recompute the solution recursively
         }
         else if (R <= epsilon_64)
            dt *= two; // increase the time step for future steps
      }

   private:
      static constexpr auto two = static_cast<value_t>(2.0);
      static constexpr auto fifth = static_cast<value_t>(1.0 / 5.0);

      static constexpr auto half = static_cast<value_t>(0.5);
      static constexpr auto third = static_cast<value_t>(1.0 / 3.0);
      static constexpr auto sixth = static_cast<value_t>(1.0 / 6.0);
      static constexpr auto eigth = static_cast<value_t>(1.0 / 8.0);

      state_t x0, xd0, xd2, xd3, xd_temp;
      const value_t epsilon, epsilon_64;
      state_t x_estimate;
   };
}