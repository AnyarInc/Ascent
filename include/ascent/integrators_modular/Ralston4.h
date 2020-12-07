// Copyright (c) 2016-2020 Anyar, Inc.
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

// Ralston's fourth-order method.

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct Ralston4prop : public Propagator<value_t>
      {
         void operator()(State& state, const value_t dt) override
         {
            auto& x = *state.x;
            auto& xd = *state.xd;
            state.memory.resize(5);
            auto& x0 = state.memory[0];
            auto& xd0 = state.memory[1];
            auto& xd1 = state.memory[2];
            auto& xd2 = state.memory[3];
            auto& xd3 = state.memory[4];

            switch (Propagator<value_t>::pass)
            {
            case 0:
               x0 = x;
               xd0 = xd;  // k1
               x = x0 + dt * (0.4 * xd0);
               break;
            case 1:
               xd1 = xd;  // k2
               x = x0 + dt * (0.29697761 * xd0 + 0.15875964 * xd1);
               break;
            case 2:
               xd2 = xd;  // k3
               x = x0 + dt * (0.21810040 * xd0 - 3.05096516 * xd1 + 3.83286476 * xd2);
               break;
            case 3:
               xd3 = xd; // k4
               x = x0 + dt * (0.17476028 * xd0 - 0.55148066 * xd1 + 1.20553560 * xd2 + 0.17118478 * xd3);
               break;
            }
         }
      };

      template <class value_t>
      struct Ralston4stepper : public TimeStepper<value_t>
      {
         value_t t0{};

         void operator()(const size_t pass, value_t& t, const value_t dt) override
         {
            switch (pass)
            {
            case 0:
               t0 = t;
               t = t0 + 0.4 * dt;
               break;
            case 1:
               t = t0 + 0.45573725 * dt;
               break;
            case 2:
               t = t0 + dt;
               break;
            default:
               break;
            }
         }
      };

      template <class value_t>
      struct Ralston4
      {
         static constexpr size_t n_substeps = 4;
         
         asc::Module* run_first{};
		 
         Ralston4prop<value_t> propagator;
         Ralston4stepper<value_t> stepper;

         template <class modules_t>
         void operator()(modules_t& blocks, value_t& t, const value_t dt)
         {
            auto& pass = propagator.pass;
            pass = 0;
            
            update(blocks, run_first);
            apply(blocks);
            propagate(blocks, propagator, dt);
            stepper(pass, t, dt);
            postprop(blocks);
            ++pass;

            update(blocks, run_first);
            apply(blocks);
            propagate(blocks, propagator, dt);
            postprop(blocks);
            ++pass;

            update(blocks, run_first);
            apply(blocks);
            propagate(blocks, propagator, dt);
            stepper(pass, t, dt);
            postprop(blocks);
            ++pass;

            update(blocks, run_first);
            apply(blocks);
            propagate(blocks, propagator, dt);
            postprop(blocks);
         }
      };
   }
}
