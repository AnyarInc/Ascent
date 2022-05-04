// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

// Simple Euler integration.

namespace asc
{
   template <typename state_t>
   struct euler_t
   {
      using value_t = typename state_t::value_type;

      template <typename System>
      void operator()(System&& system, state_t& x, value_t& t, const value_t dt)
      {
         const size_t n = x.size();
         if (xd.size() < n)
            xd.resize(n);

         system(x, xd, t);
         for (size_t i = 0; i < n; ++i) {
            x[i] += dt * xd[i];
         }
         t += dt;
      }

   private:
      state_t xd;
   };
}
