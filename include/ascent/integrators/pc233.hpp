// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

#include "ascent/utility.hpp"
#include "ascent/integrators/rk2.hpp"

// P-2/PC-3/C-3 algorithm, which has the same error coefficient and order as the P-3/PC-3/C-3 predictor-corrector, but is more stable

// Real Time (RT) Adam's Moulton predictor-corrector integration
// Source: R.M. Howe. A new family of real-time predictor-corrector integration algorithms. The University of Michigan. September 1991.

namespace asc
{
   template <typename state_t, typename init_integrator = rk4_t<state_t>>
   struct pc233_t
   {
      using value_type = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_type& t, const value_type dt)
      {
         if (!initialized)
         {
            initializer(system, x, t, dt);
            xd_1 = initializer.xd;
            initialized = true;
            return;
         }

         const auto t0 = t;
         const auto dt_3 = cx<value_type>(1.0 / 3.0)*dt;

         const size_t n = x.size();
         if (xd0.size() < n)
         {
            xd0.resize(n);
            xd_temp.resize(n);
         }

         x0 = x;
         system(x0, xd0, t);
         size_t i{};
         for (; i < n; ++i)
            x[i] = x0[i] + c0 * dt * (7.0*xd0[i] - xd_1[i]); // X(n + 1/3), third step computation
         t += dt_3;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + c1 * dt * (39.0*xd_temp[i] - 4.0*xd0[i] + xd_1[i]); // X(n + 2/3), two thirds step computation
         t += dt_3;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + c2 * dt * (xd0[i] + 3.0*xd_temp[i]);
         xd_1 = xd0;
         t = t0 + dt;
      }

   private:
      bool initialized{};
      init_integrator initializer;
      state_t x0, xd0, xd_temp;
      state_t xd_1; // -1, previous time step derivative

      static constexpr auto c0 = cx<value_type>(1.0 / 18.0);
      static constexpr auto c1 = cx<value_type>(1.0 / 54.0);
      static constexpr auto c2 = cx<value_type>(1.0 / 4.0);
   };
}
