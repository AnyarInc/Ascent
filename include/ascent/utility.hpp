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
   template <class value_t, class v_t>
   inline constexpr auto cx(const v_t v) { return static_cast<value_t>(v); }

   template <typename T>
   struct adaptive_t
   {
      T abs_tol = static_cast<T>(1.0); // absolute tolerance
      T rel_tol = static_cast<T>(1.0); // relative tolerance
      T safety_factor = static_cast<T>(0.9); // value < 1.0 reduces time step change aggressiveness
   };
}
