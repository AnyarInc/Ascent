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

#include "State.h"
#include <initializer_list>
#include <array>
#include <deque>
#include <vector>

namespace asc
{
   template <typename T>
   struct VectorT
   {
      using It = typename std::vector<T>::iterator;

      VectorT(const VectorT& vec) = default;

      VectorT& operator=(const VectorT& v)
      {
         for (size_t i = 0; i < n; ++i)
            this->operator[](i) = v[i];
         return *this;
      }

      template <typename C>
      VectorT& operator=(const C& c)
      {
         for (size_t i = 0; i < n; ++i)
            this->operator[](i) = c[i];
         return *this;
      }

      template <typename C>
      VectorT(C& c, const size_t n) : n(n), data_ptr(c.data() + c.size())
      {
         StateT<T>(c, T());
         i0 = --c.end() - c.begin();

         for (size_t i = 1; i < n; ++i)
            StateT<T>(c, T());
      }

      template <typename C>
      VectorT(C& c, std::initializer_list<T>&& list) : n(list.size()), data_ptr(c.data() + c.size())
      {
         bool first_element{ true };
         for (T x : list)
         {
            StateT<T>(c, x);

            if (first_element)
            {
               first_element = false;

               i0 = --c.end() - c.begin();
            }
         }
      }

      // Constructor for selecting a specific section of allocated memory
      VectorT(const size_t i0, const size_t n, T* data) : i0(i0), n(n), data_ptr(data) {}

      template <typename C>
      VectorT operator()(C& xd) const
      {
         return VectorT(i0, n, xd.data() + i0);
      }

      const T* begin() const { return data_ptr; }
      const T* end() const { return data_ptr + n; }

      T& operator[](const size_t i) const { return *(data_ptr + i); }

      size_t size() const { return n; }

      T* data() const { return data_ptr; }

      void zero()
      {
         for (size_t i = 0; i < n; ++i)
            this->operator[](i) = T();
      }

   private:
      size_t i0{}; // staring index
      const size_t n;
      T* data_ptr;
   };
}