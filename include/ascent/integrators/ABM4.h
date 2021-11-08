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

#include <array>

namespace asc
{
   /// Fourth order Adams-Bashforth-Moulton Predictor Corrector. Fixed step version (Must reinitialize if dt is changed).
   ///
   /// \tparam state_t The state type of the system to be integrated. I.e. a std::vector or std::deque.
   template <typename state_t, typename init_integrator = RK4T<state_t>>
   struct ABM4T
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
      void operator()(System &&system, state_t &x, value_t &t, const value_t dt)
      {
         if (initialized < 3)
         {
            const auto index = 2 - initialized;
            xd_prev[index].resize(x.size());
            system(x, xd_prev[index], t);
            initializer(system, x, t, dt);
            ++initialized;
            return;
         }

         const size_t n = x.size();
         if (xd0.size() < n)
         {
            xd0.resize(n);
            xd_temp.resize(n);
         }

         x0 = x;
         system(x, xd0, t);
         size_t i{};
         for (; i < n; ++i)
            x[i] = x0[i] + c0 * dt * (55.0 * xd0[i] - 59.0 * xd_prev[0][i] + 37.0 * xd_prev[1][i] - 9.0 * xd_prev[2][i]);
         t += dt;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + c0 * dt * (9.0 * xd_temp[i] + 19.0 * xd0[i] - 5.0 * xd_prev[0][i] + xd_prev[1][i]);

         for (i = 2; i > 0; --i)
            xd_prev[i] = xd_prev[i - 1];
         xd_prev[0] = xd0;
      }

   private:
      int initialized{};
      init_integrator initializer;
      state_t x0, xd0, xd_temp;
      std::array<state_t, 3> xd_prev; //previous time step derivative

      static constexpr auto c0 = cx(1.0 / 24.0);
   };
}