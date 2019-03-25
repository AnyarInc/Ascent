// Copyright (c) 2016-2019 Anyar, Inc.
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

#include <cmath>
#include "ascent/timing/Sampler.h"
#include "ascent/modular/Module.h"

namespace asc
{
   template <class value_t>
   struct Timing : Module
   {
      value_t t{};
      value_t dt{ 0.01 };
      value_t t_end = std::numeric_limits<value_t>::max();
      value_t t_delta{};

      value_t t_previous{};
      value_t eps = static_cast<value_t>(1.0e-8);
      bool time_advanced = false;

      SamplerT<value_t> sampler{ t, dt };

      bool sample(const value_t sample_rate) noexcept
      {
         return sampler(sample_rate);
      }

      bool event(const value_t event_time) noexcept
      {
         return sampler.event(event_time);
      }

      void base_time_step(const value_t base_dt)
      {
         sampler.base_time_step(base_dt);
      }

      value_t base_time_step() const noexcept
      {
         return sampler.base_time_step();
      }

      value_t delta_t() const noexcept
      {
         return t_delta;
      }

      void reset() noexcept
      {
         sampler.reset();
      }

      void init() override
      {
         t_previous = t;
      }

      void operator()() override
      {
         if (t > t_previous + eps)
         {
            time_advanced = true;
         }
         else
         {
            time_advanced = false;
         }
         t_delta = t - t_previous;
         t_previous = t;
      }
   };
}