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

namespace asc
{
   /// Fourth order, four pass Runge Kutta integrator.
   ///
   /// Designed for optimum speed and minimal memory load. We use one additional multiplication to reduce memory cost by 25%.
   /// \tparam state_t The state type of the system to be integrated. I.e. a std::vector or std::deque.
   template <typename state_t>
   struct RK4T
   {
      using value_t = typename state_t::value_type;

      /// \brief Integration step operation
      /// 
      /// Steps the system a single time step (dt), internally advances time (t)
      ///
      /// \tparam System The system object or function type. The system to be stepped must have a function syntax that takes \c (state_t, state_t, value_t) \c (x, xd, t).
      /// The system can be a function, functor, or lambda expression. Taken as an rvalue reference.
      /// \param[in, out] system The \c System instance.
      /// \param[in, out] x The system's state vector.
      /// \param[in, out] t The current time, which will be advanced by c\ dt.
      /// \param[in] dt The time step for the input system to be advanced.
      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, const value_t dt)
      {
         const value_t t0 = t;
         const value_t dt_2 = 0.5_v*dt;
         const value_t dt_6 = cx(1.0 / 6.0)*dt;

         const size_t n = x.size();
         if (xd.size() < n)
         {
            xd.resize(n);
            xd_temp.resize(n);
         }

         x0 = x;
         system(x0, xd, t);
         size_t i{};
         for (; i < n; ++i)
            x[i] = dt_2 * xd[i] + x0[i];
         t += dt_2;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
         {
            xd[i] += 2 * xd_temp[i];
            x[i] = dt_2 * xd_temp[i] + x0[i];
         }

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
         {
            xd[i] += 2 * xd_temp[i];
            x[i] = dt * xd_temp[i] + x0[i];
         }
         t = t0 + dt;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = dt_6 * (xd[i] + xd_temp[i]) + x0[i];
      }

   private:
      state_t x0, xd, xd_temp;
   };
}