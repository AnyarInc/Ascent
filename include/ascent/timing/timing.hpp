// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

#include <cmath>
#include "ascent/timing/sampler.hpp"
#include "ascent/modular/block.hpp"

namespace asc
{
   template <class value_t>
   struct timing_t : block_t
   {
      value_t t{};
      value_t dt{ 0.01 };
      value_t t_end = std::numeric_limits<value_t>::max();
      value_t t_delta{};
      
      value_t t_previous{};
      value_t eps = static_cast<value_t>(1.0e-8);
      bool time_advanced = false;

      sampler_t<value_t> sampler{ t, dt };

      bool sample(const value_t sample_rate) const noexcept {
         return sampler(sample_rate);
      }

      bool event(const value_t event_time) noexcept {
         return sampler.event(event_time);
      }

      void base_time_step(const value_t base_dt) {
         sampler.base_time_step(base_dt);
      }
      
      auto base_time_step() const noexcept {
         return sampler.base_time_step();
      }

      auto delta_t() const noexcept {
         return t_delta;
      }
      
      void reset() noexcept {
         sampler.reset();
      }

      void init() override {
         t_previous = t;
      }

      void operator()() override
      {
         if (t > t_previous + eps) {
            time_advanced = true;
         }
         else {
            time_advanced = false;
         }
         t_delta = t - t_previous;
         t_previous = t;
      }
   };
}
