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
   // TimeAdvanced must be called at the end of the update sequence, otherwise there will be no difference between the current time and the previous time
   template <typename value_t>
   struct TimeAdvancedT
   {
      value_t t_previous{};
      value_t eps = static_cast<value_t>(1.0e-8);

      bool operator()(const value_t t) const noexcept
      {
         if (t > t_previous + eps)
            return true;
         return false;
      }

      double delta_t(const value_t t) const noexcept
      {
         return t - t_previous;
      }

      void update(const value_t t) noexcept
      {
         if (operator()(t))
            t_previous = t;
      }

      template <class state_t>
      void operator()(state_t&, state_t&, const value_t t) noexcept
      {
         if (operator()(t))
            t_previous = t;
      }
   };
}