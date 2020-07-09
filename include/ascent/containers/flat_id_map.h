// Copyright (c) 2016-2020 Anyar, Inc.
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

#include <vector>

namespace asc
{
   // A flat map that is as fast to iterate through as possible and uses a binary search to locate elements.
   // Important: Keys must be emplaced back in a sorted manner.
   template <class Key, class Value>
   struct flat_id_map
   {
      using iterator = typename std::vector<std::pair<Key, Value>>::iterator;
      using const_iterator = typename std::vector<std::pair<Key, Value>>::const_iterator;

      template <class... Args>
      void emplace_back(Args&&... args)
      {
         vec.emplace_back(std::forward<Args>(args)...);
      }

      void reserve(const size_t n) { vec.reserve(n); }

      void erase(const size_t i)
      {
         const auto it = std::lower_bound(vec.cbegin(), vec.cend(), std::pair<size_t, Value>{ i, Value{} }, [=](const auto& p0, const auto& p1) { return p0.first < p1.first; });
         vec.erase(it);
      }

      iterator find(const size_t i)
      {
         return std::lower_bound(vec.cbegin(), vec.cend(), std::pair<size_t, Value>{ i, Value{} }, [=](const auto& p0, const auto& p1) { return p0.first < p1.first; });
      }

      auto& operator[](const size_t i)
      {
         const auto it = std::lower_bound(vec.cbegin(), vec.cend(), std::pair<size_t, Value>{ i, Value{} }, [=](const auto& p0, const auto& p1) { return p0.first < p1.first; });
         return it->second;
      }

      const auto& operator[](const size_t i) const
      {
         const auto it = std::lower_bound(vec.cbegin(), vec.cend(), std::pair<size_t, Value>{ i, Value{} }, [=](const auto& p0, const auto& p1) { return p0.first < p1.first; });
         return it->second;
      }

      iterator begin() noexcept { return vec.begin(); }
      iterator end() noexcept { return vec.end(); }

      const_iterator begin() const noexcept { return vec.cbegin(); }
      const_iterator cbegin() const noexcept { return vec.cbegin(); }
      const_iterator end() const noexcept { return vec.cend(); }
      const_iterator cend() const noexcept { return vec.cend(); }

   private:
      std::vector<std::pair<Key, Value>> vec;
   };
}
