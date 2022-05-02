// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

#include "ascent/utility.hpp"

// Second order, two pass Runge Kutta.

namespace asc
{
   template <class state_t>
   struct rk2_t final
   {
      using value_type = typename state_t::value_type;

      template <class system_t>
      void operator()(system_t&& system, state_t& x, value_type& t, const value_type dt) noexcept(noexcept(std::forward<system_t>(system)(std::declval<state_t&>(), std::declval<state_t&>(), std::declval<const value_type>())))
      {
         const auto t0 = t;
         const auto dt_2 = 0.5 * dt;

         const size_t n = x.size();
         if (xd.size() < n) {
            xd.resize(n);
         }

         x0 = x;
         system(x0, xd, t);
         size_t i{};
         for (; i < n; ++i) {
            x[i] = dt_2 * xd[i] + x0[i];
         }
         t += dt_2;

         system(x, xd, t);
         for (i = 0; i < n; ++i) {
            x[i] = dt * xd[i] + x0[i];
         }
         t = t0 + dt;
      }

      state_t xd;

   private:
      state_t x0;
   };
}
