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

#include "param.hpp"
#include <initializer_list>
#include <vector>

// The param_v_t acts on a contiguous array of data

namespace asc
{
   template <class T>
   struct param_v_t
   {
      using iterator = typename std::vector<T>::iterator;

      param_v_t(param_v_t&& other) : i0(std::move(other.i0)), n(std::move(other.n)), data_ptr(std::move(other.data_ptr)) {}
      param_v_t(const param_v_t& other) : i0(other.i0), n(other.n), data_ptr(other.data_ptr) {}

      param_v_t& operator=(const param_v_t& v)
      {
         for (size_t i = 0; i < n; ++i)
            this->operator[](i) = v[i];
         return *this;
      }

      template <typename C>
      param_v_t& operator=(const C& c)
      {
         for (size_t i = 0; i < n; ++i)
            this->operator[](i) = c[i];
         return *this;
      }

      template <typename C>
      param_v_t(C& c, const size_t _n) : n(_n)
      {
         param_t<T>(c, T());
         data_ptr = c.data() + c.size() - 1;
         i0 = (--c.end()) - c.begin();

         for (size_t i = 1; i < n; ++i)
            param_t<T>(c, T());
      }

      template <typename C>
      param_v_t(C& c, std::initializer_list<T>&& list) : n(list.size())
      {
         bool first_element{ true };
         for (T x : list)
         {
            param_t<T>(c, x);

            if (first_element)
            {
               first_element = false;
               data_ptr = c.data() + c.size() - 1;
               i0 = (--c.end()) - c.begin();
            }
         }
      }

      // Constructor for selecting a specific section of allocated memory
      param_v_t(const size_t _i0, const size_t _n, T* data) : i0(_i0), n(_n), data_ptr(data) {}

      template <typename C>
      param_v_t operator()(C& xd) const
      {
         return param_v_t(i0, n, xd.data() + i0);
      }

      const T* begin() const noexcept { return data_ptr; }
      const T* end() const noexcept { return data_ptr + n; }

      T& operator[](const size_t i) const noexcept { return *(data_ptr + i); }

      size_t size() const noexcept { return n; }

      T* data() const noexcept { return data_ptr; }

      void zero() noexcept
      {
         for (size_t i = 0; i < n; ++i)
            this->operator[](i) = T();
      }

   protected:
      size_t i0{}; // starting index
      const size_t n;
      T* data_ptr;
   };
}
