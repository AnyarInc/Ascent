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

#include <functional>
#include <vector>

namespace asc
{
   template <typename state_t, typename system_t>
   struct SystemT
   {
      void push_back(const system_t& func)
      {
         functions.push_back(func);
      }

      void operator()(const state_t& x, state_t& xd, const double t)
      {
         for (auto& f : functions)
            f(x, xd, t);
      }

      std::vector<system_t> functions;
   };
}