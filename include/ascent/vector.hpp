// Copyright (c) 2016-2018 Anyar, Inc.
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

#include "ascent/ParamV.h"

namespace asc
{
   template <size_t N, class value_t = double>
   struct Vector : asc::ParamVT<value_t>
   {
      Vector(std::vector<value_t>& c) : asc::ParamVT<value_t>(c, N) {}
      Vector(Vector&& other) : asc::ParamVT<value_t>(std::move(other)) {}
      Vector(const value_t& other) : asc::ParamVT<value_t>(other) {}

      using value_type = value_t;

      template <typename C>
      Vector(C& c, std::initializer_list<value_t>&& list) : asc::ParamVT<value_t>(c, list) {}

      Vector(const size_t i0, const size_t n, value_t* data) : asc::ParamVT<value_t>(i0, n, data) {}

      Vector& operator=(const Vector& v)
      {
         for (size_t i = 0; i < n; ++i)
            this->operator[](i) = v[i];
         return *this;
      }

      Vector& operator=(const std::vector<value_t>& v)
      {
         for (size_t i = 0; i < n; ++i)
            this->operator[](i) = v[i];
         return *this;
      }

      using asc::ParamVT<value_t>::operator[];

      double norm() const
      {
         const size_t n = size();
         value_t sum{};
         for (size_t i = 0; i < n; ++i)
            sum += operator[](i) * operator[](i);
         return sqrt(sum);
      }

      std::vector<value_t> normalized() const
      {
         std::vector<value_t> ret;
         ret.reserve(N);
         const value_t magnitude = norm();
         for (size_t i = 0; i < N; ++i)
         {
            ret.emplace_back(operator[](i) / magnitude);
         }
         return ret;
      }
   };

   inline auto operator-(const Vector<3>& lhs, const Vector<3>& rhs) -> std::vector<Vector<3>::value_type>
   {
      const size_t n = lhs.size();
      std::vector<Vector<3>::value_type> ret(n);
      for (size_t i = 0; i < n; ++i)
         ret[i] = lhs[i] - rhs[i];
      return ret;
   }
}
