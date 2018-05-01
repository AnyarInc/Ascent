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

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

// A threaded worker queue

namespace asc
{
   struct Queue final
   {
      Queue() = default;
      Queue(const Queue&) = default;
      Queue(Queue&&) = default;
      Queue& operator=(const Queue&) = default;
      Queue& operator=(Queue&&) = default;
      ~Queue() { run = false; }

      bool computing() const noexcept
      {
         return running_jobs;
      }

      template <typename ...Args>
      void emplace_back(Args&&... args)
      {
         adding = true;
         jobs.emplace_back(std::forward<Args>(args)...);
         adding = false;
         if (!started)
         {
            start();
            started = true;
         }
         cv.notify_one();
      }

      size_t size() const
      {
         return jobs.size();
      }

      std::function<void()> work_done = [] {}; // Do not assign to work_done while jobs are running

      template <typename ChaiScript>
      static void script(ChaiScript& c, const std::string& name)
      {
         using namespace chaiscript;
         using T = Queue;
         c.add(constructor<T()>(), name);

         c.add(fun(&T::work_done), "work_done");
         c.add(fun(&T::computing), "computing");
         c.add(fun(&T::emplace_back<std::function<void()>>), "emplace_back");
         c.add(fun(&T::size), "size");
      }

   private:
      void start()
      {
         run = true;

         auto runner = [&]
         {
            while (run)
            {
               running_jobs = true;
               while (jobs.size() > 0)
               {
                  jobs.front()();

                  while (adding)
                     std::this_thread::yield();
                  jobs.pop_front();
               }
               running_jobs = false;
               work_done();

               std::unique_lock<std::mutex> lock(m);
               cv.wait(lock);
            }
         };

         std::thread(runner).detach();
      }

      std::mutex m;
      bool started{};
      std::atomic<bool> run{}, adding{}, running_jobs{};
      std::condition_variable cv;
      std::deque<std::function<void()>> jobs;
   };
}