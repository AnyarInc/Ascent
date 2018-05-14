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

// Second order, two pass Runge Kutta.

namespace asc
{
   template <typename state_t>
   struct RK2T
   {
      using value_t = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, const value_t dt)
      {
         const value_t t0 = t;
         const value_t dt_2 = 0.5_v*dt;

         const size_t n = x.size();
         if (xd.size() < n)
            xd.resize(n);

         x0 = x;
         system(x0, xd, t);
         size_t i{};
         for (; i < n; ++i)
            x[i] = dt_2 * xd[i] + x0[i];
         t += dt_2;

         system(x, xd, t);
         for (i = 0; i < n; ++i)
            x[i] = dt * xd[i] + x0[i];
         t = t0 + dt;
      }

      state_t xd;

   private:
      state_t x0;
   };
}