// Copyright (c) 2016-2018 Anyar, Inc.
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

// Modified Euler Midpoint integration

namespace asc
{
   template <typename state_t>
   struct MidpointT
   {
      using value_t = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, const value_t dt)
      {
         const size_t n = x.size();
         if (xd.size() < n)
         {
            xd.resize(n);
            xd_new.resize(n);
         }

         system(x, xd_new, t);
         for (size_t i = 0; i < n; ++i)
         {
            x[i] += 0.5 * dt * (xd_new[i] + xd[i]);
         }
         xd = xd_new;
         t += dt;
      }

   private:
      state_t xd, xd_new;
   };
}