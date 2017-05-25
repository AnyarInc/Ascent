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

namespace asc
{
   // x is the independant variable, y is dependent. Returns derivative for unequally spaced points. xest is the x value at which to evaluate the derivative.
   // x and y vectors don't need to be the same length if they have three elements or more each
   template <typename T0, typename T1>
   inline auto derivative(const T0& x, const T1& y, const typename T0::value_type xest)
   {
      const size_t nx = x.size();
      const size_t ny = y.size();
      if (x.size() < 2 || y.size() < 2)
         return typename T0::value_type();
      else if (nx == 2 || ny == 2)
         return (y.back() - y.front()) / (x.back() - x.front());
      else
      {
         const typename T0::value_type x2 = x[nx - 1];
         const typename T0::value_type x1 = x[nx - 2];
         const typename T0::value_type x0 = x[nx - 3];

         const typename T1::value_type y2 = y[ny - 1];
         const typename T1::value_type y1 = y[ny - 2];
         const typename T1::value_type y0 = y[ny - 3];

         const typename T0::value_type dydx = y0*(2.0 * xest - x1 - x2) / ((x0 - x1)*(x0 - x2)) + y1*(2.0 * xest - x0 - x2) / ((x1 - x0)*(x1 - x2)) + y2*(2.0 * xest - x0 - x1) / ((x2 - x0)*(x2 - x1));

         return dydx;
      }
   }

   // x is the independant variable, y is dependent. Returns derivative for unequally spaced points.
   // Uses the last x value for the prediction point.
   template <typename T0, typename T1>
   inline auto derivative(const T0& x, const T1& y)
   {
      return derivative(x, y, x.back());
   }

   // E is intended to be an Eigen::Vector, such as Eigen::Vector3d, supports n dimensional Eigen vectors
   template <typename E, typename T0, typename T1>
   inline E derivative_vector(const T0 &t, const T1 &v)
   {
      const size_t n = t.size();
      if (n < 2)
         return E::Zero();
      else if (n == 2)
         return (v[1] - v[0]) / (t[1] - t[0]);

      std::vector<std::vector<double>> dimensional_history; // each vector is for a dimension, such as x, y, z, . . . (can have more dimensions than 3), saving three time steps of history
      const size_t dimensions = v[0].rows();
      dimensional_history.resize(dimensions);
      for (size_t i = 0; i < dimensions; ++i)
      {
         for (size_t j = 3; j > 0; --j) // iterate over last three states, from oldest to newest
            dimensional_history[i].push_back(v[n - j][i]); // this will push back the ith states (i.e. if i = 0, x dimension) from the last three Eigen::Vector states
      }

      E deriv;
      for (size_t i = 0; i < dimensions; ++i)
         deriv[i] = derivative(t, dimensional_history[i]); // t can contain more than 3 steps

      return deriv;
   }
}