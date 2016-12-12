// Copyright (c) 2016 Anyar, Inc.
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

#include "ascent/core/Propagate.h"

// Fourth order, four pass Runge Kutta.

namespace asc
{
//   namespace core
//   {
//      // a specialized function to enable vectorization
//      template <typename T, typename C>
//      inline void propRK4(C& x, const T dt, const C& xd0, const C& xd1, const C& xd2, const C& xd3, const C& x0)
//      {
//         const size_t n = xd0.size();
//         static constexpr T sixth = static_cast<T>(1.0 / 6.0);
//         static constexpr T two = static_cast<T>(2.0);
//#ifdef ASCENT_NO_FMA
//         for (size_t i = 0; i < n; ++i)
//            x[i] = sixth * dt * (xd0[i] + two * (xd1[i] + xd2[i]) + xd3[i]) + x0[i];
//#else
//         for (size_t i = 0; i < n; ++i)
//            x[i] = fma(sixth * dt, xd0[i] + fma(two, xd1[i] + xd2[i], xd[i]), x0[i]);
//#endif
//      }
//   }

   template <typename C>
   struct RK4T
   {
      template <typename Clock, typename System>
      void operator()(Clock& clock, C& x, System&& system)
      {
         auto& t = clock.t;
         t0 = clock.t;
         dt = clock.dt;
         dt_2 = half*dt;

         const size_t n = x.size();
         if (xd0.size() < n)
         {
            xd0.resize(n);
            xd1.resize(n);
            xd2.resize(n);
            xd3.resize(n);
         }

         for (size_t pass = 0; pass < 4; ++pass)
         {
            switch (pass)
            {
            case 0:
               x0 = x;
               system(x, xd0, t);
               core::propagate(x, dt_2, xd0, x0);
               t += dt_2;
               break;
            case 1:
               system(x, xd1, t);
               core::propagate(x, dt_2, xd1, x0);
               break;
            case 2:
               system(x, xd2, t);
               core::propagate(x, dt, xd2, x0);
               t = t0 + dt;
               break;
            case 3:
               system(x, xd3, t);
#ifdef ASCENT_NO_FMA
               for (size_t i = 0; i < n; ++i)
                  x[i] = sixth * dt * (xd0[i] + two * (xd1[i] + xd2[i]) + xd3[i]) + x0[i];
#else
               for (size_t i = 0; i < n; ++i)
                  x[i] = fma(sixth * dt, xd0[i] + fma(two, xd1[i] + xd2[i], xd[i]), x0[i]);
#endif
               break;
            default:
               break;
            }
         }
      }

   private:
      static constexpr typename C::value_type two = static_cast<typename C::value_type>(2.0);
      static constexpr typename C::value_type half = static_cast<typename C::value_type>(0.5);
      static constexpr typename C::value_type sixth = static_cast<typename C::value_type>(1.0 / 6.0);
      typename C::value_type t0, dt, dt_2;
      C x0, xd0, xd1, xd2, xd3;
   };
}