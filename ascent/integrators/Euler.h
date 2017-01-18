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

// Simple Euler integration.

namespace asc
{
   template <typename state_t>
   struct EulerT
   {
      using value_t = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, const value_t dt)
      {
         const size_t n = x.size();
         if (xd.size() < n)
            xd.resize(n);

         system(x, xd, t);
         for (size_t i = 0; i < n; ++i)
         {
            x[i] += dt * xd[i];
         }
         t += dt;
      }

   private:
      state_t xd;
   };
}