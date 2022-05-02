// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#pragma once

// A param is a reference to a memory location within a system's state

namespace asc
{
   namespace core
   {
      template <class C>
      inline typename C::value_type& emplace_back_ref(C& c, const typename C::value_type x0) {
         // TODO: Check if can grow without allocating
         return c.emplace_back(x0);
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
      T& operator()(C& xd) const noexcept {
         return xd[index];
      }

      // Returns true if this State is within a given state array
      template <typename C>
      bool within(const C& vec) const noexcept {
         return &x == &vec[index];
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
