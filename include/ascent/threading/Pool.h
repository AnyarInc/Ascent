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

#include <thread>
#include <deque>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace asc {
   // A simple threadpool
   class Pool
   {
   public:
      Pool(const unsigned int n = std::thread::hardware_concurrency())
      {
         n_threads(n);
      }
      void n_threads(const unsigned int n)
      {
         for (size_t i = threads.size(); i < n; ++i)
            threads.emplace_back(std::thread(&Pool::worker, this));
      }
	  void emplace_back(std::function<void()>&& task)
	  {
		  std::lock_guard<std::mutex> lock(m);
		  queue.emplace_back(std::forward<std::function<void()>>(task));
		  work_cv.notify_one();
	  }
      bool computing() const
      {
         return (working != 0);
      }
      void wait() {
         std::unique_lock<std::mutex> lock(m);
         if (queue.empty() && (working == 0))
            return;
         done_cv.wait(lock, [&]() { return queue.empty() && (working == 0); });
      }
	  size_t size() const
	  {
		  return threads.size();
	  }
      ~Pool()
      {
         //Close the queue and finish all the remaining work
         std::unique_lock<std::mutex> lock(m);
         closed = true;
         work_cv.notify_all();
         lock.unlock();

         for (auto& t : threads)
            if (t.joinable())
               t.join();
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
         c.add(fun(&T::emplace_back), "emplace_back");
         c.add(fun(&T::size), "size");
      }

   private:
      std::vector< std::thread > threads;
      std::deque< std::function<void()> > queue;
      std::atomic<unsigned int> working = 0;
      bool closed = false;
      std::mutex m;
      std::condition_variable work_cv;
      std::condition_variable done_cv;

      void worker()
      {
         while (true)
         {
            //Wait for work
            std::unique_lock<std::mutex> lock(m);
            work_cv.wait(lock, [this]() { return closed || !queue.empty(); });
            if (queue.empty()) {
               if (closed) {
                  return;
               }
               continue;
            }

            //Grab work
            ++working;
            auto work = queue.front();
            queue.pop_front();
            lock.unlock();

            work();

            //Notify that work is finished
            lock.lock();
            --working;
            done_cv.notify_all();
         }
      }
   };
}