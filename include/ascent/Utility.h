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

#include <vector>

namespace asc
{
   // User type definitions for ease of use
   using value_t = double; // float, double, etc.
   using state_t = std::vector<value_t>; // std::vector, std::deque, etc.

   constexpr const value_t cx(long double v) { return static_cast<value_t>(v); }
   constexpr const value_t operator"" _v(long double v) { return static_cast<value_t>(v); }

   struct AdaptiveIntegrator
   {
      AdaptiveIntegrator() = default;
      AdaptiveIntegrator(const AdaptiveIntegrator&) = default;
      AdaptiveIntegrator(AdaptiveIntegrator&&) = default;
      AdaptiveIntegrator& operator=(const AdaptiveIntegrator&) = default;
      AdaptiveIntegrator& operator=(AdaptiveIntegrator&&) = default;
      virtual ~AdaptiveIntegrator() = default;
   };

   template <typename T>
   struct AdaptiveT
   {
      T abs_tol = static_cast<T>(1.0); // absolute tolerance
      T rel_tol = static_cast<T>(1.0); // relative tolerance
      T safety_factor = static_cast<T>(0.9); // value < 1.0 reduces time step change aggressiveness
   };
}