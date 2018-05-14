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

#include "ascent/Recorder.h"
#include "ascent/Param.h"

// Timing
#include "ascent/timing/Sampler.h"

// Integrators
#include "ascent/integrators/Euler.h"
#include "ascent/integrators/Midpoint.h"
#include "ascent/integrators/RK2.h"
#include "ascent/integrators/RK4.h"
#include "ascent/integrators/DOPRI45.h"
#include "ascent/integrators/RTAM4.h"
#include "ascent/integrators/PC233.h"

// Linear Algebra
#include "ascent/ParamV.h"

#include "ascent/System.h"

#include <deque>
#include <string>

// Type definitions for cleaner code

namespace asc
{
   // Edits To The Following Types Are Not Reccommended
   // ------------------------------------------------
   using system_t = std::function<void(const state_t&, state_t&, const value_t)>;

   using System = SystemT<state_t, system_t>;

   using Recorder = RecorderT<value_t>;
   using RecorderString = RecorderT<std::string>;
   using Sampler = SamplerT<value_t>;
   using Param = ParamT<value_t>;

   // Integrators
   using Euler = EulerT<state_t>;
   using Midpoint = MidpointT<state_t>;
   using RK2 = RK2T<state_t>;
   using RK4 = RK4T<state_t>;
   using DOPRI45 = DOPRI45T<state_t>;
   using PC233 = PC233T<state_t>;

   // Linear Algebra
   using ParamV = ParamVT<value_t>;
}