// Copyright (c) 2016-2019 Anyar, Inc.
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

#include <vector>

namespace asc
{
   // pointers, rather than references are used for the possibility of developing states with shared_ptrs for x and xd
   struct State
   {
      State(double& x, double& xd) noexcept : x(&x), xd(&xd) {}
      State(const State&) = default;
      State(State&&) = default;
      State& operator=(const State&) = default;
      State& operator=(State&&) = default;

      double* x{};
      double* xd{};

      std::vector<double> memory;
   };

   template <class states_t, class x_t, class xd_t>
   inline void make_states(states_t& states, x_t& x, xd_t& xd)
   {
      const size_t n = x.size();
      for (size_t i = 0; i < n; ++i)
      {
         states.emplace_back(x[i], xd[i]);
      }
   }
}