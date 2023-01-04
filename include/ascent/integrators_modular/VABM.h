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

#include "ascent/Utility.h"
#include "ascent/integrators_modular/ModularIntegrators.h"
#include "ascent/integrators_modular/RK4.h"
#include "ascent/timing/Timing.h"

#include <cmath>

// Variable step Adams-Bashforth-Moulton predictor corrector of configurable order.
// This is based on Krogh's Variable Step Adams Formulas and the implementation in OrdinaryDiffEq.jl
namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct VABMprop : public Propagator<value_t>
      {
         VABMprop(size_t order = 4)
         {
            set_order(order);
         }

         void calc_beta(size_t k) {
            value_t xi = dt[0];
            value_t xi_0 = 0.0;
            beta[0] = 1.0;
            for (size_t i = 1; i < k; ++i) {
               xi_0 += dt[i];
               beta[i] = beta[i - 1] * xi / xi_0;
               xi += dt[i];
            }
         }

         void calc_phi(State &state, value_t dx, size_t k) {
            const auto phi_n_a = &state.memory[phi_n_i]; // Array length 4
            const auto phi_star_n_a = &state.memory[phi_star_n_i]; // Array length 4
            const auto phi_star_nm1_a = &state.memory[phi_star_nm1_i]; // Array length 4
            const auto phi_n = [&](size_t i) -> auto &{return *(phi_n_a + i); };
            const auto phi_star_n = [&](size_t i) -> auto &{return *(phi_star_n_a + i); };
            const auto phi_star_nm1 = [&](size_t i) -> auto &{return *(phi_star_nm1_a + i); };

            phi_n(0) = dx;
            phi_star_n(0) = dx;
            for (size_t i = 1; i < k; ++i) {
               phi_n(i) = phi_n(i - 1) - phi_star_nm1(i - 1);
               phi_star_n(i) = beta[i] * phi_n(i);
            }
         }

         void calc_phi_np1(State &state, value_t dx, size_t k) {
            const auto phi_star_n_a = &state.memory[phi_star_n_i]; // Array length 4
            const auto phi_np1_a = &state.memory[phi_np1_i]; // Array length 4
            const auto phi_star_n = [&](size_t i) -> auto &{return *(phi_star_n_a + i); };
            const auto phi_np1 = [&](size_t i) -> auto &{return *(phi_np1_a + i); };

            phi_np1(0) = dx;
            for (size_t i = 1; i < k; ++i) {
               phi_np1(i) = phi_np1(i - 1) - phi_star_n(i - 1);
            }
         }

         void calc_g(size_t k) {
            value_t xi = 0.0;
            for (size_t i = 0; i < k; ++i) {
               if (i > 0) {
                  xi += dt[i - 1];
               }
               for (size_t j = 0; j < (k - i); ++j) {
                  size_t q = j + 1;
                  if (i == 0) {
                     c[i][j] = 1.0 / q;
                  }
                  else if (i == 1) {
                     c[i][j] = 1.0 / (q * (q + 1));
                  }
                  else {
                     c[i][j] = (-dt[0] / xi) * c[i - 1][j + 1] + c[i - 1][j];
                  }
               }
               g[i] = c[i][0] * dt[0];
            }
         }

         void swap_phi_star(State &state) {
            const auto phi_star_n_a = &state.memory[phi_star_n_i]; // Array length k
            const auto phi_star_nm1_a = &state.memory[phi_star_nm1_i]; // Array length k
            for (size_t i = 0; i < order; ++i) {
               std::swap(*(phi_star_n_a + i), *(phi_star_nm1_a + i));
            }
         }

         void operator()(State &state, const value_t /*dt*/) override
         {
            auto &x = *state.x;
            auto &xd = *state.xd;
            if (state.memory.size() < memory_size) {
               state.memory.resize(memory_size);
            }
            auto &x0 = state.memory[x0_i];
            auto &xd0 = state.memory[xd0_i];
            auto &xp = state.memory[xp_i];
            const auto phi_n_a = &state.memory[phi_n_i]; // Array length k
            const auto phi_star_n_a = &state.memory[phi_star_n_i]; // Array length k
            const auto phi_star_nm1_a = &state.memory[phi_star_nm1_i]; // Array length k
            const auto phi_np1_a = &state.memory[phi_np1_i]; // Array length k+1
            [[maybe_unused]] const auto phi_n = [&](size_t i) -> auto &{return *(phi_n_a + i); };
            const auto phi_star_n = [&](size_t i) -> auto &{return *(phi_star_n_a + i); };
            [[maybe_unused]] const auto phi_star_nm1 = [&](size_t i) -> auto &{return *(phi_star_nm1_a + i); };
            const auto phi_np1 = [&](size_t i) -> auto &{return *(phi_np1_a + i); };

            switch (Propagator<value_t>::pass)
            {
            case 0:
               x0 = x;
               calc_phi(state, xd, order);
               for (size_t i = 0; i < order; ++i) {
                  x += g[i] * phi_star_n(i);
               }
               xd0 = xd;
               xp = x;
               break;
            case 1:
               calc_phi_np1(state, xd, order + 1);
               x += g[order] * phi_np1(order);
               swap_phi_star(state);
               break;
            }
         }

         std::vector<value_t> dt;
         std::vector<value_t> g;
         static constexpr size_t x0_i = 0;
         static constexpr size_t xd0_i = 1;
         static constexpr size_t xp_i = 2;
         static constexpr size_t phi_n_i = 3;
         size_t phi_star_n_i{};
         size_t phi_star_nm1_i{};
         size_t phi_np1_i{};
         size_t memory_size{};

      private:

         void set_order(size_t k) {
            // This is private becuase we currently dont support dynamicly changing the order
            order = k;
            size_t k1 = order + 1;
            dt.resize(order);
            beta.resize(order);
            g.resize(k1);
            c.resize(k1);
            for (size_t i = 0; i < k1; ++i) {
               c[i].resize(k1);
            }
            phi_star_n_i = 3 + k;
            phi_star_nm1_i = phi_star_n_i + k;
            phi_np1_i = phi_star_nm1_i + k;
            memory_size = phi_np1_i + k + 1;
         }

         size_t order{};
         std::vector<value_t> beta;
         std::vector<std::vector<value_t>> c;
      };

      template <class value_t>
      struct VABMstepper : public TimeStepper<value_t>
      {
         void operator()(const size_t pass, value_t &t, const value_t dt) override
         {
            switch (pass)
            {
            case 0:
               t += dt;
               break;
            case 1:
               break;
            }
         }
      };

      /// Variable step Adams-Bashforth-Moulton predictor corrector of configurable order.
      template <typename value_t, typename init_integrator = RK4<value_t>>
      //struct VABM : AdaptiveIntegrator
      struct VABM
      {
         VABM(size_t order = 4) : order(order), propagator(order) {};

         template <typename modules_t>
         void operator()(modules_t &blocks, value_t &t, const value_t dt)
         {
            if (initialized < (order - 1))
            {
               if (run_first)
               {
                  initializer.run_first = run_first;
               }

               // Record full step state history
               if (initialized == 0) {
                  if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
                  {
                     for (auto &block : blocks)
                     {
                        for (auto &state : block.second->states)
                        {
                           state.hist_len = order - 1;
                        }
                     }
                  }
                  else
                  {
                     for (auto &block : blocks)
                     {
                        for (auto &state : block->states)
                        {
                           state.hist_len = order - 1;
                        }
                     }
                  }
               }

               // Run initializer integrator
               initializer(blocks, t, dt);

               propagator.dt[(order - 2) - initialized] = dt;

               ++initialized;

               if (initialized == (order - 1)) {
                  propagator.calc_beta(order);

                  // calc_phi for past steps and store in the states memory
                  if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
                  {
                     for (auto &block : blocks)
                     {
                        for (auto &state : block.second->states)
                        {
                           if (state.memory.size() < propagator.memory_size) {
                              state.memory.resize(propagator.memory_size);
                           }
                           for (size_t k = 0; k < order - 1; ++k) {
                              propagator.calc_phi(state, state.xd0_hist[k], k + 1);
                              propagator.swap_phi_star(state);
                              state.hist_len = 0; // We dont need to track the history anymore
                           }
                        }
                     }
                  }
                  else
                  {
                     for (auto &block : blocks)
                     {
                        for (auto &state : block->states)
                        {
                           if (state.memory.size() < propagator.memory_size) {
                              state.memory.resize(propagator.memory_size);
                           }
                           for (size_t k = 0; k < order - 1; ++k) {
                              propagator.calc_phi(state, state.xd0_hist[k], k + 1);
                              propagator.swap_phi_star(state);
                              state.hist_len = 0; // We dont need to track the history anymore
                           }
                        }
                     }
                  }
               }
               return;
            }

            for (size_t i = propagator.dt.size() - 1; i > 0; --i) {
               propagator.dt[i] = propagator.dt[i - 1];
            }
            propagator.dt[0] = dt;

            propagator.calc_g(order + 1);
            propagator.calc_beta(order);

            auto &pass = propagator.pass;
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
            stepper(pass, t, dt);
            postprop(blocks);
         }

         template <class modules_t>
         void operator()(modules_t &blocks, value_t &t, value_t &dt, const AdaptiveT<value_t> &settings)
         {
            const value_t abs_tol = settings.abs_tol;
            const value_t rel_tol = settings.rel_tol;
            [[maybe_unused]] const value_t safety_factor = settings.safety_factor;

            const value_t t0 = t;

         start_vabm_adaptive:

            size_t init = initialized;
            operator()(blocks, t, dt);
            if (init < order - 1) {
               return;
            }

            value_t e_max{};
            for (auto &block : blocks)
            {
               auto solve = [&](auto &state)
               {
                  [[maybe_unused]] auto &x = *state.x;
                  [[maybe_unused]] auto &xd = *state.xd;
                  auto &x0 = state.memory[propagator.x0_i];
                  [[maybe_unused]] auto &xd0 = state.memory[propagator.xd0_i];
                  const auto phi_np1_a = &state.memory[propagator.phi_np1_i];
                  const auto phi_np1 = [&](size_t i) -> auto &{return *(phi_np1_a + i); };

                  value_t lte = std::abs((propagator.g[order] - propagator.g[order - 1]) * phi_np1(order));
                  //value_t lte_m1 = std::abs((propagator.g[order - 1] - propagator.g[order - 2]) * phi_np1(order - 1));
                  //std::cout << lte / lte_m1 << '\n';

                  //value_t e = lte / (abs_tol + rel_tol * (std::abs(x0) + 0.01 * std::abs(xd0)));
                  value_t e = lte / (abs_tol + rel_tol * (std::abs(x0)));
                  //value_t e = std::max(lte / abs_tol, lte / (rel_tol * std::abs(x0)));

                  if (e > e_max)
                  {
                     e_max = e;
                  }
               };

               if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
               {
                  for (auto &state : block.second->states)
                  {
                     solve(state);
                  }
               }
               else
               {
                  for (auto &state : block->states)
                  {
                     solve(state);
                  }
               }
            }

            if (e_max > 1.0)
            {
               dt *= std::max(std::pow(2.0 * e_max, -cx(1.0 / (order + 1))), 0.5_v);

               if (run_first) {
                  run_first->base_time_step(dt);
               }

               t = t0;

               for (size_t i = 0; i < order - 1; ++i) {
                  propagator.dt[i] = propagator.dt[i + 1];
               }

               for (auto &block : blocks)
               {
                  auto solve = [&](auto &state)
                  {
                     *state.x = state.memory[propagator.x0_i];
                     propagator.swap_phi_star(state);
                  };

                  if constexpr (is_pair_v<typename std::iterator_traits<typename modules_t::iterator>::value_type>)
                  {
                     for (auto &state : block.second->states)
                     {
                        solve(state);
                     }
                  }
                  else
                  {
                     for (auto &state : block->states)
                     {
                        solve(state);
                     }
                  }
               }

               goto start_vabm_adaptive; // recompute the solution recursively
            }

            if (e_max < 0.25_v)
            {
               e_max = std::max(1e-7, e_max);
               dt *= std::min(std::pow(2.0 * e_max, -cx(1.0 / (order + 1))), 1.5_v);

               if (run_first) {
                  run_first->base_time_step(dt);
               }
            }
         }

         asc::Timing<double> *run_first{};

      private:
         size_t order{};
         size_t initialized = 0;
         init_integrator initializer;
         VABMprop<value_t> propagator;
         VABMstepper<value_t> stepper;
      };
   }
}