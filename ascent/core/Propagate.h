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

#define ASCENT_NO_FMA
// With current compilers, not using fma allows the loop to be vectorized. In the future the loop may be able to be vectorized with fma.
// If speed is not as important as accuracy, enabling fma can reduce rounding errors.

namespace asc
{
   namespace core
   {
      template <typename T, typename C>
      inline void propagate(C& x, const T dt, const C& xd, const C& x0)
      {
         const size_t n = xd.size();
#ifdef ASCENT_NO_FMA
         for (size_t i = 0; i < n; ++i)
            x[i] = dt * xd[i] + x0[i];
#else
         for (size_t i = 0; i < n; ++i)
            x[i] = fma(dt, xd[i], x0[i]);
#endif
      }

      template <typename T, typename C>
      inline void propagate(C& x, const T dt, const C& xd0, const C& xd, const C& x0)
      {
         const size_t n = xd.size();
#ifdef ASCENT_NO_FMA
         for (size_t i = 0; i < n; ++i)
            x[i] = dt * (xd0[i] + xd[i]) + x0[i];
#else
         for (size_t i = 0; i < n; ++i)
            x[i] = fma(dt, xd0[i] + xd[i], x0[i]);
#endif
      }
   }
}
