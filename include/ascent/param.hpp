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

// A Param is a reference to a memory location within the system's state

namespace asc
{
   namespace core
   {
      template <class C>
      inline typename C::value_type& emplace_back_ref(C& c, const typename C::value_type x0)
      {
         c.emplace_back(x0);
         return c.back();
      }
   }

   template <class T>
   struct param_t
   {
      template <class C>
      param_t(C& c, const T x0 = T()) : index(c.size()), x(core::emplace_back_ref(c, x0)) {}

      param_t(const param_t&) = default;
      param_t(param_t&&) = default;
      param_t& operator=(const param_t&) = default;
      param_t& operator=(param_t&&) = default;

      template <typename C>
      T& operator()(C& xd) const noexcept { return xd[index]; }

      // Returns true if this State is within a given state array
      template <typename C>
      bool within(const C& vec) const noexcept
      {
         if (&x == &vec[index])
            return true;
         return false;
      }

      operator T&() const noexcept { return x; }

      T& operator=(const T y) noexcept { x = y; return x; }

   private:
      const size_t index;
      T& x;
   };
   
   template <class C>
   param_t(C& c, const typename C::value_type x0 = typename C::value_type()) -> param_t<typename C::value_type>;
}
