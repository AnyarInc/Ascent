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

#include "ascent/threading/Queue.h"

// a multi-threaded pool of queues, with simple load balancing

namespace asc
{
   struct Pool final
   {
      std::vector<std::unique_ptr<Queue>> pool;

      bool computing() const
      {
         for (auto& q : pool)
         {
            if (q->computing())
               return true;
         }
         return false;
      }

      void n_threads(const size_t n)
      {
         for (size_t i = pool.size(); i < n; ++i)
            pool.emplace_back(std::make_unique<Queue>());
      }

      void assign_callback(const std::function<void()>& f)
      {
         for (auto& q : pool)
            q->work_done = f;
      }

      template <typename ...Args>
      void emplace_back(Args&&... args)
      {
         if (pool.empty())
            pool.emplace_back(std::make_unique<Queue>());
         auto compare = [](auto& l, auto& r) { return l->size() < r->size(); };
         auto it = std::min_element(pool.begin(), pool.end(), compare);
         (*it)->emplace_back(args...);
      }

      size_t size() const
      {
         return pool.size();
      }

      template <typename ChaiScript>
      static void script(ChaiScript& c, const std::string& name)
      {
         using namespace chaiscript;
         using T = Pool;
         c.add(constructor<T()>(), name);

         c.add(fun(&T::computing), "computing");
         c.add(fun(&T::n_threads), "n_threads");
         c.add(fun(&T::assign_callback), "assign_callback");
         c.add(fun(&T::emplace_back<std::function<void()>>), "emplace_back");
         c.add(fun([](T& pool, std::function<void()> func) { pool.emplace_back(func); }), "push_back");
         c.add(fun(&T::size), "size");
      }
   };
}