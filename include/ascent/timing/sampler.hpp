// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

#include <cmath>

namespace asc
{
   // A sampler_t resets the base time step when it goes out of scope. In this manner it can be used as a short-lived object within the simulation loop.
   // If the sampler_t is to exist long term, then the reset() must be called after time is incremented within the simulation loop.
   // Note that keeping the Sampler around longer maintains a consistent base time step.
   template <class value_t>
   struct sampler_t
   {
      sampler_t(const value_t _t, const value_t _dt, const value_t _eps = 1.0e-8) noexcept : t(_t), dt(_dt), dt_base(dt), eps(_eps) {}
      sampler_t(const sampler_t& other) : t(other.t), dt(other.dt), dt_base(other.dt_base) {}
      sampler_t(sampler_t&&) = default;
      sampler_t& operator=(const sampler_t& other)
      {
         t = other.t;
         dt = other.dt;
         dt_base = other.dt_base;
         return *this;
      }
      sampler_t& operator=(sampler_t&&) = default;
      ~sampler_t() noexcept { dt = dt_base; }

      // sample returns true at 0.0 or the sample rate
      bool sample(const value_t sample_rate) const noexcept
      {
         const auto n = static_cast<size_t>((t + eps) / sample_rate); // the number of sample time steps that have occured
         const auto sample_time = (n + 1) * sample_rate; // the next sample time
         if (sample_time < t + dt - eps) {
            dt = sample_time - t;
         }

         return t - sample_time + sample_rate < eps;
      }
      
      // event steps to an exact time
      bool event(const value_t event_time) noexcept
      {
         if (event_time < t + dt - eps && event_time >= t + eps) {
            dt = event_time - t;
         }

         return std::abs(event_time - t) < eps;
      }

      void reset() noexcept {
         dt = dt_base;
      }

      value_t base_time_step() const noexcept {
         return dt_base;
      }

      void base_time_step(const double dt_new) noexcept {
         dt = dt_base = dt_new;
      }

   private:
      value_t& t;
      value_t& dt;
      value_t dt_base;
      value_t eps = 1.0e-8;
   };
}
