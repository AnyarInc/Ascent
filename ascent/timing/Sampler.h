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
   // A Sampler resets the base time step when it goes out of scope. In this manner it can be used as a short-lived object within the simulation loop.
   // If the Sampler is to exist long term, then the reset() must be called after time is incremented within the simulation loop.
   // Note that keeping the Sampler around longer maintains a consistent base time step.
   template <typename T>
   struct SamplerT
   {
      SamplerT(const T& t, T& dt) noexcept : t(t), dt(dt), dt_base(dt) {}
      SamplerT(const SamplerT&) = default;
      SamplerT(SamplerT&&) = default;
      SamplerT& operator=(const SamplerT&) = default;
      SamplerT& operator=(SamplerT&&) = default;
      ~SamplerT() noexcept { dt = dt_base; }

      bool operator()(const T sample_rate) noexcept
      {
         const size_t n = static_cast<size_t>((t + eps) / sample_rate); // the number of sample time steps that have occured
         const T sample_time = (n + 1) * sample_rate; // the next sample time
         if (sample_time < t + dt - eps)
            dt = sample_time - t;

         if (t - sample_time + sample_rate < eps)
            return true;

         return false;
      }

      bool event(const T event_time) noexcept
      {
         if (event_time < t + dt - eps && event_time >= t + eps)
            dt = event_time - t;

         if (fabs(event_time - t) < eps)
            return true;

         return false;
      }

      void reset() noexcept
      {
         dt = dt_base;
      }

   private:
      static constexpr T eps = static_cast<T>(1.0e-10);
      const T& t;
      T& dt;
      const T dt_base;
   };
}