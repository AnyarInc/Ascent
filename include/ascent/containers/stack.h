// Copyright (c) 2016-2018 Anyar, Inc.
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

// An optimized stack container for Recorders

#include <deque>
#include <vector>

namespace asc
{
   template <class T, class stack_t>
   struct stack_iterator
   {
      stack_iterator(const size_t i, stack_t& stack) noexcept : index(i % stack_t::block_size), slice(i / stack_t::block_size), stack(stack), ptr(stack.data(i)) {}

      stack_iterator(const stack_iterator&) noexcept = default;
      stack_iterator(stack_iterator&&) noexcept = default;
      stack_iterator& operator=(const stack_iterator&) noexcept = default;
      stack_iterator& operator=(stack_iterator&&) noexcept = default;

      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator_category = std::forward_iterator_tag;

      T& operator*()
      {
         return *ptr;
      }

      const T& operator*() const
      {
         return *ptr;
      }

      pointer operator->() noexcept
      {
         return ptr;
      }

      const pointer operator->() const noexcept
      {
         return ptr;
      }

      stack_iterator& operator++() noexcept
      {
         ++index;
         ++ptr;
         if (index == stack_t::block_size)
         {
            index = 0;
            ++slice;
            ptr = stack.data_slice(slice);
         }
         return *this;
      }

      bool operator!=(const stack_iterator& rhs) const noexcept
      {
         if (index != rhs.index || slice != rhs.slice)
            return true;
         return false;
      }

      bool operator==(const stack_iterator& rhs) const noexcept
      {
         if (index == rhs.index && slice == rhs.slice)
            return true;
         return false;
      }

   private:
      size_t index{};
      size_t slice{};
      stack_t& stack;
      T* ptr;
   };

   template <class T, class stack_t>
   struct const_stack_iterator
   {
      const_stack_iterator(const size_t i, stack_t& stack) noexcept : index(i % stack_t::block_size), slice(i / stack_t::block_size), stack(stack), ptr(stack.data(i)) {}

      const_stack_iterator(const const_stack_iterator&) noexcept = default;
      const_stack_iterator(const_stack_iterator&&) noexcept = default;
      const_stack_iterator& operator=(const const_stack_iterator&) noexcept = default;
      const_stack_iterator& operator=(const_stack_iterator&&) noexcept = default;

      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator_category = std::forward_iterator_tag;

      const T& operator*() const
      {
         return *ptr;
      }

      const pointer operator->() const
      {
         return ptr;
      }

      const_stack_iterator& operator++() noexcept
      {
         ++index;
         ++ptr;
         if (index == stack_t::block_size)
         {
            index = 0;
            ++slice;
            ptr = stack.data_slice(slice);
         }
         return *this;
      }

      bool operator!=(const const_stack_iterator& rhs) const noexcept
      {
         if (index != rhs.index || slice != rhs.slice)
            return true;
         return false;
      }

      bool operator==(const const_stack_iterator& rhs) const noexcept
      {
         if (index == rhs.index && slice == rhs.slice)
            return true;
         return false;
      }

   private:
      size_t index{};
      size_t slice{};
      stack_t& stack;
      T* ptr;
   };

   template <class T, size_t block>
   struct stack final
   {
      stack() noexcept
      {
         deq.emplace_back();
         deq.back().reserve(block);
      }

      stack(const stack& other)
      {
         deq = other.deq;
         deq.back().reserve(block); // is not by default copied to the same size
         slice = other.slice;
      }
      stack(stack&&) = default;
      stack& operator=(const stack& other)
      {
         deq = other.deq;
         deq.back().reserve(block); // capacity is not by default copied to the same size
         slice = other.slice;
         return *this;
      }
      stack& operator=(stack&&) = default;

      static constexpr size_t block_size = block;

      using iterator = stack_iterator<T, stack<T, block>>;
      using const_iterator = const_stack_iterator<T, stack<T, block>>;

      iterator begin()
      {
         return{ 0, *this };
      }

      iterator end()
      {
         return{ size(), *this };
      }

      const_iterator cbegin()
      {
         return{ size(), *this };
      }

      const_iterator cend()
      {
         return{ size(), *this };
      }

      void emplace_back() noexcept
      {
         emplace_back(T());
      }
      
      template <class Arg>
      void emplace_back(Arg&& x) noexcept
      {
         auto& s = deq[slice];
         if (s.size() == block)
         {
            deq.emplace_back();
            auto& v_back = deq.back();
            v_back.reserve(block);
            v_back.emplace_back(std::forward<Arg>(x));
            ++slice;
         }
         else
         {
            s.emplace_back(std::forward<Arg>(x));
         }
      }

      T& operator[](const size_t i) noexcept
      {
         return deq[i / block][i % block];
      }

      const T& operator[](const size_t i) const noexcept
      {
         return deq[i / block][i % block];
      }

      T& operator()(const size_t slice_index, const size_t index) noexcept
      {
         return deq[slice_index][index];
      }

      const T& operator()(const size_t slice_index, const size_t index) const noexcept
      {
         return deq[slice_index][index];
      }

      T& front()
      {
         return deq.front().front();
      }

      const T& front() const
      {
         return deq.front().front();
      }

      T& back()
      {
         return deq.back().back();
      }

      const T& back() const
      {
         return deq.back().back();
      }

      size_t size() const noexcept
      {
         return (deq.size() - 1) * block + deq.back().size();
      }

      T* data_slice(const size_t s)
      {
         if (s >= deq.size())
            return nullptr;
         return deq[s].data();
      }

      const T* data_slice(const size_t s) const
      {
         if (s >= deq.size())
            return nullptr;
         return deq[s].data();
      }

      T* data(const size_t i)
      {
         if (i / block >= deq.size())
            return nullptr;
         return deq[i / block].data() + i % block;
      }

      const T* data(const size_t i) const
      {
         if (i / block >= deq.size())
            return nullptr;
         return deq[i / block].data() + i % block;
      }

      void clear()
      {
         deq.resize(1);
         deq.front().resize(0); // we don't want to remove the capacity on the vector by calling clear
      }

   private:
      std::deque<std::vector<T>> deq;
      size_t slice{};
   };
}
