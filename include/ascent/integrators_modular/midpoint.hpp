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

#include "ascent/modular/Module.h"
#include "ascent/integrators_modular/ModularIntegrators.h"

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct MidpointProp : public Propagator<value_t>
      {
         void operator()(State& state, const value_t dt) override
         {
            if (state.memory.empty())
            {
               state.memory.resize(1);
               state.memory[0] = 0.0;
            }
            auto& xd0 = state.memory[0];
            
            auto& xd = *state.xd;
            *state.x += 0.5 * dt * (xd0 + xd);
            xd0 = xd;
         }
      };
   }
}
