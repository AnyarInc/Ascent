// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

// Modified Euler Midpoint integration

namespace asc
{
   template <typename state_t>
   struct midpoint_t
   {
      using value_t = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, const value_t dt)
      {
         const size_t n = x.size();
         if (xd.size() < n)
         {
            xd.resize(n);
            xd_new.resize(n);
         }

         system(x, xd_new, t);
         for (size_t i = 0; i < n; ++i)
         {
            x[i] += 0.5 * dt * (xd_new[i] + xd[i]);
         }
         xd = xd_new;
         t += dt;
      }

   private:
      state_t xd, xd_new;
   };
}
