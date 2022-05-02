// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

// UNDER CONSTRUCTION

#include "ascent/integrators/rk4.hpp"

namespace asc
{
   template <typename state_t>
   struct RTAM4T
   {
      using value_type = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_type& t, const value_type dt)
      {
         if (!initialized)
         {
            initializer(system, x, t, dt);
         }

         const auto t0 = t;

         const size_t n = x.size();
         if (xd.size() < n)
         {
            xd.resize(n);
            xd0.resize(n);
            xd_1.resize(n);
            xd_2.resize(n);
            xd_3.resize(n);
         }

         x0 = x;
         system(x0, xd, t);
         xd0 = xd;
         size_t i;
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c0*xd[i] + c1*xd_1[i] + c2*xd_2[i] + c3*xd_3[i]);
         t += 0.5 * dt;

         system(x, xd, t);
         for (i = 0; i < n; ++i)
            x[i] = x0[i] + dt * (c4*xd[i] + c5*xd0[i] + c6*xd_1[i] + c7*xd_2[i]);
         t = t0 + dt;

         xd_3 = xd_2;
         xd_2 = xd_1;
         xd_1 = xd0;
      }

   private:
      bool initialized{};

      rk4_t<state_t> initializer;

      static constexpr auto c0 = cx<value_type>(297.0 / 384.0);
      static constexpr auto c1 = cx<value_type>(-187.0 / 384.0);
      static constexpr auto c2 = cx<value_type>(107.0 / 384.0);
      static constexpr auto c3 = cx<value_type>(-25.0 / 384.0);

      static constexpr auto c4 = cx<value_type>(36.0 / 30.0);
      static constexpr auto c5 = cx<value_type>(-10.0 / 30.0);
      static constexpr auto c6 = cx<value_type>(5.0 / 30.0);
      static constexpr auto c7 = cx<value_type>(-1.0 / 30.0);

      state_t x0, xd, xd0, xd_1, xd_2, xd_3;
   };
}
