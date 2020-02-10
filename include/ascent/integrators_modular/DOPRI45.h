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

#include "ascent/Utility.h"

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct DOPRI45prop : public Propagator<value_t>
      {
         void operator()(State& state, const value_t dt) override
         {
            const auto dt_5 = 0.2_v * dt;

            auto& x = *state.x;
            auto& xd = *state.xd;
            if (state.memory.size() < 7)
            {
               state.memory.resize(7);
            }
            auto& x0 = state.memory[0];
            auto& xd0 = state.memory[1];
            auto& xd_temp = state.memory[2];  // xd_temp is used for xd1 and xd5
            auto& xd2 = state.memory[3];
            auto& xd3 = state.memory[4];
            auto& xd4 = state.memory[5];
            auto& xd6 = state.memory[6];

            switch (Propagator<value_t>::pass)
            {
            case 0:
               x0 = x;
               xd0 = xd;
               x = x0 + dt_5 * xd0;
               break;
            case 1:
               xd_temp = xd;
               x = x0 + dt * (c10 * xd0 + c11 * xd_temp);
               break;
            case 2:
               xd2 = xd;
               x = x0 + dt * (c20 * xd0 + c21 * xd_temp + c22 * xd2);
               break;
            case 3:
               xd3 = xd;
               x = x0 + dt * (c30 * xd0 + c31 * xd_temp + c32 * xd2 + c33 * xd3);
            case 4:
               xd4 = xd;
               x = x0 + dt * (c40 * xd0 + c41 * xd_temp + c42 * xd2 + c43 * xd3 + c44 * xd4);
               break;
            case 5:
               xd_temp = xd;
               x = x0 + dt * (c50 * xd0 + c52 * xd2 + c53 * xd3 + c54 * xd4 + c55 * xd_temp);
               break;
            default:
               break;
            }
         }

      private:
         static constexpr auto c10 = cx(3.0 / 40.0);
         static constexpr auto c11 = cx(9.0 / 40.0);

         static constexpr auto c20 = cx(44.0 / 45.0);
         static constexpr auto c21 = cx(-56.0 / 15.0);
         static constexpr auto c22 = cx(32.0 / 9.0);

         static constexpr auto c30 = cx(19372.0 / 6561.0);
         static constexpr auto c31 = cx(-25360.0 / 2187.0);
         static constexpr auto c32 = cx(64448.0 / 6561.0);
         static constexpr auto c33 = cx(-212.0 / 729.0);

         static constexpr auto c40 = cx(9017.0 / 3168.0);
         static constexpr auto c41 = cx(-355.0 / 33.0);
         static constexpr auto c42 = cx(46732.0 / 5247.0);
         static constexpr auto c43 = cx(49.0 / 176.0);
         static constexpr auto c44 = cx(-5103.0 / 18656.0);

         static constexpr auto c50 = cx(35.0 / 384.0);
         // c51 is 0
         static constexpr auto c52 = cx(500.0 / 1113.0);
         static constexpr auto c53 = cx(125.0 / 192.0);
         static constexpr auto c54 = cx(-2187.0 / 6784.0);
         static constexpr auto c55 = cx(11.0 / 84.0);
      };

      template <class value_t>
      struct DOPRI45stepper : public TimeStepper<value_t>
      {
         value_t t0{};

         void operator()(const size_t pass, value_t& t, const value_t dt) override
         {
            switch (pass)
            {
            case 0:
               t0 = t;
               t += 0.2_v * dt;
               break;
            case 1:
               t = t0 + cx(3.0 / 10.0) * dt;
               break;
            case 2:
               t = t0 + cx(4.0 / 5.0) * dt;
               break;
            case 3:
               t = t0 + cx(8.0 / 9.0) * dt;
               break;
            case 4:
               t = t0 + dt;
               break;
            default:
               break;
            }
         }
      };

      template <class value_t>
      struct DOPRI45 : AdaptiveIntegrator
      {
         DOPRI45prop<value_t> propagator;
         DOPRI45stepper<value_t> stepper;

         bool fsal_computed = false;

         asc::Timing<double>* run_first{};

         template <class modules_t>
         void system(modules_t& blocks, value_t& t, const value_t dt)
         {
            if (!fsal_computed) // if an adaptive stepper hasn't computed the first same as last state, we must compute the step here
            {
               update(blocks);

               auto solve = [&](auto& state)
               {
                  auto& xd = *state.xd;
                  if (state.memory.size() < 7)
                  {
                     state.memory.resize(7);
                  }
                  auto& xd0 = state.memory[1];
                  xd0 = xd;
               };

               for (auto& block : blocks)
               {
                  if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
                  {
                     for (auto& state : block.second->states)
                     {
                        solve(state);
                     }
                  }
                  else
                  {
                     for (auto& state : block->states)
                     {
                        solve(state);
                     }
                  }
               }
               fsal_computed = false;
            }

            auto& pass = propagator.pass;
            pass = 0;

            for (auto i = 0; i < 5; ++i)
            {
               update(blocks);
               propagate(blocks, propagator, dt);
               stepper(pass, t, dt);
               postprop(blocks);
               ++pass;
            }

            update(blocks);
            propagate(blocks, propagator, dt);
            postprop(blocks);
         }

         template <class modules_t>
         void operator()(modules_t& blocks, value_t& t, value_t& dt, const AdaptiveT<value_t>& settings)
         {
            const value_t abs_tol = settings.abs_tol;
            const value_t rel_tol = settings.rel_tol;
            const value_t safety_factor = settings.safety_factor;

            const value_t t0 = t;
         
         start_adaptive:
            system(blocks, t, dt);

            // xd6 is xd0, because first same as last (FSAL)
            update(blocks);
            for (auto& block : blocks)
            {
               auto solve = [&](auto& state)
               {
                  auto& xd = *state.xd;
                  if (state.memory.size() < 7)
                  {
                     state.memory.resize(7);
                  }
                  auto& xd6 = state.memory[6];
                  xd6 = xd;
               };

               if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
               {
                  for (auto& state : block.second->states)
                  {
                     solve(state);
                  }
               }
               else
               {
                  for (auto& state : block->states)
                  {
                     solve(state);
                  }
               }
            }

            value_t e_max{};
            for (auto& block : blocks)
            {
               auto solve = [&](auto& state)
               {
                  auto& x = *state.x;
                  auto& xd = *state.xd;
                  if (state.memory.size() < 7)
                  {
                     state.memory.resize(7);
                  }
                  auto& x0 = state.memory[0];
                  auto& xd0 = state.memory[1];
                  auto& xd_temp = state.memory[2];  // xd_temp is used for xd1 and xd5
                  auto& xd2 = state.memory[3];
                  auto& xd3 = state.memory[4];
                  auto& xd4 = state.memory[5];
                  auto& xd6 = state.memory[6];

                  // overwrite xd2 as the error estimate, this saves memory
                  xd2 = abs(x0 + dt * (e0 * xd0 + e2 * xd2 + e3 * xd3 + e4 * xd4 + e5 * xd_temp + e6 * xd6) - x); // absolute error estimate (x4th - x5th)

                  value_t e = xd2 / (abs_tol + rel_tol * (abs(x0) + 0.01 * abs(xd0)));

                  if (e > e_max)
                  {
                     e_max = e;
                  }
               };

               if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
               {
                  for (auto& state : block.second->states)
                  {
                     solve(state);
                  }
               }
               else
               {
                  for (auto& state : block->states)
                  {
                     solve(state);
                  }
               }
            }

            if (e_max > 1.0)
            {
               dt *= std::max(0.9_v * pow(e_max, -cx(1.0 / 3.0)), 0.2_v);
               run_first->base_time_step(dt);

               t = t0;

               for (auto& block : blocks)
               {
                  auto solve = [&](auto& state)
                  {
                     *state.x = state.memory[0];
                  };

                  if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
                  {
                     for (auto& state : block.second->states)
                     {
                        solve(state);
                     }
                  }
                  else
                  {
                     for (auto& state : block->states)
                     {
                        solve(state);
                     }
                  }
               }

               goto start_adaptive; // recompute the solution recursively
            }

            if (e_max < 0.5_v)
            {
               e_max = std::max(3.2e-4_v, e_max); // 3.2e-4 = pow(5, -5)
               dt *= 0.9_v * pow(e_max, -0.2_v);
               run_first->base_time_step(dt);
            }

            for (auto& block : blocks)
            {
               auto solve = [&](auto& state)
               {
                  auto& xd = *state.xd;
                  if (state.memory.size() < 7)
                  {
                     state.memory.resize(7);
                  }
                  auto& xd0 = state.memory[1];
                  auto& xd6 = state.memory[6];
                  xd0 = xd6;
               };

               if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
               {
                  for (auto& state : block.second->states)
                  {
                     solve(state);
                  }
               }
               else
               {
                  for (auto& state : block->states)
                  {
                     solve(state);
                  }
               }
            }
            
            fsal_computed = true;
         }

      private:
         static constexpr auto e0 = cx(5179.0 / 57600.0);
         // e1 is 0
         static constexpr auto e2 = cx(7571.0 / 16695.0);
         static constexpr auto e3 = cx(393.0 / 640.0);
         static constexpr auto e4 = cx(-92097.0 / 339200.0);
         static constexpr auto e5 = cx(187.0 / 2100.0);
         static constexpr auto e6 = cx(1.0 / 40.0);
      };
   }
}
