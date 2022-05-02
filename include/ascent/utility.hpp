// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

#include <utility>

namespace asc
{
   template <class value_t, class v_t>
   inline constexpr auto cx(const v_t v) { return static_cast<value_t>(v); }

   template <class T>
   struct adaptive_t
   {
      T abs_tol = static_cast<T>(1.0); // absolute tolerance
      T rel_tol = static_cast<T>(1.0); // relative tolerance
      T safety_factor = static_cast<T>(0.9); // value < 1.0 reduces time step change aggressiveness
   };
}
