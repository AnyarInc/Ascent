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
      Pool() = default;
      Pool(const Pool&) = default;
      Pool(Pool&&) = default;
      Pool& operator=(const Pool&) = default;
      Pool& operator=(Pool&&) = default;
      ~Pool()
      {
         run_balancer = false;
         cv_balancer.notify_one();
      }

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
         {
            pool.emplace_back(std::make_unique<Queue>());
            pool.back()->pool_balancer = &cv_balancer;
         }
      }

      void wait()
      {
         for (auto& q : pool)
         {
            if (q->computing())
            {
               q->wait();
            }
         }
      }

      template <typename ...Args>
      void emplace_back(Args&&... args)
      {
         if (pool.empty())
         {
            pool.emplace_back(std::make_unique<Queue>());
            pool.back()->pool_balancer = &cv_balancer;
         }
         auto compare = [](auto& l, auto& r) { return l->size() < r->size(); };
         auto it = std::min_element(pool.begin(), pool.end(), compare);
         (*it)->emplace_back(std::forward<Args>(args)...);

         if (!balancer_active)
         {
            balancer_active = true;

            auto balancer = [&]
            {
               while (run_balancer)
               {
                  size_t min_size = std::numeric_limits<size_t>::max();
                  size_t max_size{};
                  size_t min_index{};
                  size_t max_index{};

                  for (auto i = 0; i < pool.size(); ++i)
                  {
                     const auto n = pool[i]->size();
                     if (n < min_size)
                     {
                        min_size = n;
                        min_index = i;
                     }
                     if (n > max_size)
                     {
                        max_size = n;
                        max_index = i;
                     }
                  }

                  if (max_index != min_index && max_size > 1)
                  {
                     const auto job = pool[max_index]->jobs.back();
                     pool[max_index]->jobs.pop_back();
                     pool[min_index]->emplace_back(std::move(job));
                  }

                  if (run_balancer)
                  {
                     std::unique_lock<std::mutex> lock(mtx_balancer);
                     cv_balancer.wait(lock);
                  }
               }
            };

            std::thread(balancer).detach();
         }
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
         c.add(fun(&T::wait), "wait");
         c.add(fun(&T::emplace_back<std::function<void()>>), "emplace_back");
         c.add(fun([](T& pool, const std::function<void()>& func) { pool.emplace_back(func); }), "push_back");
         c.add(fun(&T::size), "size");
      }

   private:
      std::mutex mtx_balancer;
      std::condition_variable cv_balancer;
      std::atomic<bool> balancer_active = false;
      std::atomic<bool> run_balancer = true;
   };
}