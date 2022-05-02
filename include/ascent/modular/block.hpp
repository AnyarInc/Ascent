// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

namespace asc
{
   struct block_t
   {
      block_t() noexcept = default;
      block_t(const block_t&) noexcept = default;
      block_t(block_t&&) noexcept = default;
      block_t& operator=(const block_t&) noexcept = default;
      block_t& operator=(block_t&&) noexcept = default;
      virtual ~block_t() = default;
      
      virtual void init() {} // (initialization) user is responsible for calling init
      virtual void update() {} // derivative calculations
      virtual void apply() {} // apply accumulations
      virtual void postprop() {} // post propagation calculations (every substep)
      virtual void poststep() {} // post integration calculations (every full step)
   };
}
