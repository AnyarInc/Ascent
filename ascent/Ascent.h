// Copyright (c) 2016 Anyar, Inc.
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

#include "ascent/core/AscentHeaders.h"

// Type definitions for cleaner code

namespace asc
{
   // User type definitions for ease of use
   using value_t = double; // float, double, etc.
   using state_t = std::vector<value_t>; // std::vector, std::deque, etc.


   // Edits To The Following Types Are Not Reccommended
   // ------------------------------------------------
   using system_t = std::function<void(const state_t&, state_t&, const value_t)>;

   using Clock = ClockT<value_t>;
   using Recorder = RecorderT<value_t>;
   using RecorderString = RecorderT<std::string>;
   using Sampler = SamplerT<value_t, Clock>;
   using State = StateT<value_t>;

   using Sim = SimT<state_t>;

   // Integrators
   using Euler = EulerT<state_t>;
   using RK2 = RK2T<state_t>;
   using RK4 = RK4T<state_t>;
   using RKMM = RKMMT<state_t>;

   // Linear Algebra
   using Vector = VectorT<value_t>;
}