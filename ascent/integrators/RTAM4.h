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

// UNDER CONSTRUCTION

#include "ascent/integrators/RK4.h"

namespace asc
{
   template <typename state_t>
   struct RTAM4T
   {
      using value_t = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, const value_t dt)
      {
         if (!initialized)
         {
            initializer(system, x, t, dt);
         }

         const value_t t0 = t;

         const size_t n = x.size();
         if (xd.size() < n)
         {
            xd.resize(n);
            xd0.resize(n);
            xd_1.resize(n);
            xd_2.resize(n);
            xd_3.resize(n);
         }

         x0 = x;
         system(x0, xd, t);
         xd0 = xd;
         size_t i;
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c0*xd[i] + c1*xd_1[i] + c2*xd_2[i] + c3*xd_3[i]);
         t += 0.5_v * dt;

         system(x, xd, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c4*xd[i] + c5*xd0[i] + c6*xd_1[i] + c7*xd_2[i]);
         t = t0 + dt;

         xd_3 = xd_2;
         xd_2 = xd_1;
         xd_1 = xd0;
      }

   private:
      bool initialized{};

      RK4T<state_t> initializer;

      static constexpr auto c0 = cx(297.0 / 384.0);
      static constexpr auto c1 = cx(-187.0 / 384.0);
      static constexpr auto c2 = cx(107.0 / 384.0);
      static constexpr auto c3 = cx(-25.0 / 384.0);

      static constexpr auto c4 = cx(36.0 / 30.0);
      static constexpr auto c5 = cx(-10.0 / 30.0);
      static constexpr auto c6 = cx(5.0 / 30.0);
      static constexpr auto c7 = cx(-1.0 / 30.0);

      state_t x0, xd, xd0, xd_1, xd_2, xd_3;
   };
}
