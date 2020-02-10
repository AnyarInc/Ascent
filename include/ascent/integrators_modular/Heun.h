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

// Heun's Method

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct Heunprop : public Propagator<value_t>
      {
         void operator()(State& state, const value_t dt) override
         {
            auto& x = *state.x;
            auto& xd = *state.xd;
            state.memory.resize(2);
            auto& x0 = state.memory[0];
            auto& xd0 = state.memory[1];

            switch (Propagator<value_t>::pass)
            {
            case 0:
               x0 = x;
               xd0 = xd;          //k1  
               x = x0 + dt * xd;  
               break;
            case 1:
               x = x0 + dt * 0.5 * ( xd0 + xd );
               break;
            }
         }
      };

      template <class value_t>
      struct Heunstepper : public TimeStepper<value_t>
      {
         value_t t0{};

         void operator()(const size_t pass, value_t& t, const value_t dt) override
         {
            switch (pass)
            {
            case 0:
               t0 = t;
               t += dt;
               break;
            case 1:
               t = t0 + dt;
               break;
            default:
               break;
            }
         }
      };

      template <typename value_t>
      struct Heun
      {
         asc::Timing<double>* timing{};

         Heunprop<value_t> propagator;
         Heunstepper<value_t> stepper;

         template <typename modules_t>
         void operator()(modules_t& modules, value_t& t, const value_t dt)
         {
            auto& pass = propagator.pass;
            pass = 0;

            update(modules);
            propagate(modules, dt);
            stepper(pass, t, dt);
            ++pass;

            update(modules);
            propagate(modules, dt);
            stepper(pass, t, dt);
         }

         template <class modules_t>
         void update(modules_t& modules)
         {
            if (timing)
            {
               (*timing)();
            }
            for (auto& module : modules)
            {
               (*module)();
            }
         }

         template <class modules_t>
         void propagate(modules_t& modules, const value_t dt)
         {
            for (auto& module : modules)
            {
               module->propagate(propagator, dt);
            }
         }
      };
   }
}