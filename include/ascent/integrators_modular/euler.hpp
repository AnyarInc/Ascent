// Copyright (c) 2016-2022 Anyar, Inc.
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

#include "ascent/modular/block.hpp"
#include "ascent/direct/State.h"
#include "ascent/integrators_modular/modular_integrators.hpp"

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct EulerProp : public propagator_t<value_t>
      {
         void operator()(State& state, const value_t dt) override
         {
            *state.x += dt * *state.xd;
         }
      };

      template <typename value_t>
      struct Euler
      {
         static constexpr size_t n_substeps = 1;
         
         asc::block_t* run_first{};

         EulerProp<value_t> propagator;

         template <class modules_t>
         void operator()(modules_t& blocks, value_t& t, const value_t dt)
         {
            update(blocks, run_first);
            apply(blocks);
            propagate(blocks, propagator, dt);
            t += dt;
            postprop(blocks);
         }
      };
   }
}
