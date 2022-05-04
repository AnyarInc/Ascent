// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

#include "ascent/utility.hpp"

#include <array>

namespace asc
{
   /// Fourth order Adams-Bashforth-Moulton Predictor Corrector. Fixed step version (Must reinitialize if dt is changed).
   ///
   /// \tparam state_t The state type of the system to be integrated. I.e. a std::vector or std::deque.
   template <class state_t, class init_integrator = rk4_t<state_t>>
   struct abm4_t
   {
      using value_type = typename state_t::value_type;

      template <class system_t>
      void operator()(system_t &&system, state_t &x, value_type &t, const value_type dt)
      {
         if (initialized < 3) {
            const auto index = 2 - initialized;
            xd_prev[index].resize(x.size());
            system(x, xd_prev[index], t);
            initializer(system, x, t, dt);
            ++initialized;
            return;
         }

         const size_t n = x.size();
         if (xd0.size() < n) {
            xd0.resize(n);
            xd_temp.resize(n);
         }

         x0 = x;
         system(x, xd0, t);
         size_t i{};
         for (; i < n; ++i) {
            x[i] = x0[i] + c0 * dt * (55.0 * xd0[i] - 59.0 * xd_prev[0][i] + 37.0 * xd_prev[1][i] - 9.0 * xd_prev[2][i]);
         }
         t += dt;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + c0 * dt * (9.0 * xd_temp[i] + 19.0 * xd0[i] - 5.0 * xd_prev[0][i] + xd_prev[1][i]);

         for (i = 2; i > 0; --i)
            xd_prev[i] = xd_prev[i - 1];
         xd_prev[0] = xd0;
      }

   private:
      int initialized{};
      init_integrator initializer;
      state_t x0, xd0, xd_temp;
      std::array<state_t, 3> xd_prev; //previous time step derivative

      static constexpr auto c0 = cx<value_type>(1.0 / 24.0);
   };
}
