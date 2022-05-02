// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

#include "ascent/utility.hpp"

namespace asc
{
   /// Fourth order, four pass Runge Kutta integrator.
   /// Designed for optimum speed and minimal memory load. We use one additional multiplication to reduce memory cost by 25%.
   template <class state_t>
   struct rk4_t final
   {
      using value_type = typename state_t::value_type;
      
      template <class system_t>
      void operator()(system_t&& system, state_t& x, value_type& t, const value_type dt) noexcept(noexcept(std::forward<system_t>(system)(std::declval<state_t&>(), std::declval<state_t&>(), std::declval<const value_type>())))
      {
         const auto t0 = t;
         const auto dt_2 = 0.5 * dt;
         const auto dt_6 = cx<value_type>(1.0 / 6.0) * dt;

         const size_t n = x.size();
         if (xd.size() < n) {
            xd.resize(n);
            xd_temp.resize(n);
         }

         x0 = x;
         system(x0, xd, t);
         size_t i{};
         for (; i < n; ++i) {
            x[i] = dt_2 * xd[i] + x0[i];
         }
         t += dt_2;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i) {
            xd[i] += 2 * xd_temp[i];
            x[i] = dt_2 * xd_temp[i] + x0[i];
         }
         
         system(x, xd_temp, t);
         for (i = 0; i < n; ++i) {
            xd[i] += 2 * xd_temp[i];
            x[i] = dt * xd_temp[i] + x0[i];
         }
         t = t0 + dt;

         system(x, xd_temp, t);
         for (i = 0; i < n; ++i) {
            x[i] = dt_6 * (xd[i] + xd_temp[i]) + x0[i];
         }
      }

      state_t xd;

   private:
      state_t x0, xd_temp;
   };
}
