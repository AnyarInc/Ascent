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

#include "Damper.h"
#include "Spring.h"

#include <iostream>

using namespace asc;

// The goal is to be able to implement radix sorting

struct Seq
{
   size_t N{};
   size_t id{};

   void init()
   {
      if (!initialized)
      {
         for (auto& f : run_after)
            f();
      }
      initialized = true;
   }

   void increment()
   {
      ++N;
      for (auto& f : run_after)
         f();
   }

   void before(Seq& s)
   {
      run_after.emplace_back([&] { s.increment(); });
   }

   void after(Seq& s)
   {
      s.run_before.emplace_back([&] { increment(); });
   }

private:
   bool initialized{};
   std::vector<std::function<void()>> run_before, run_after;
};

int main()
{
   asc::state_t x;
   x.reserve(100);

   std::vector<Seq> seq;
   seq.emplace_back();
   seq.emplace_back();
   seq.emplace_back();
   seq.emplace_back();

   const size_t n = seq.size();
   for (size_t i = 0; i < n; ++i)
   {
      seq[i].id = i;
   }

   seq[2].before(seq[1]);
   //seq[0].after(seq[1]);

   for (auto& s : seq)
      s.init();

   std::sort(seq.begin(), seq.end(), [](auto& lhs, auto& rhs) { return lhs.N < rhs.N; });

   for (size_t i = 0; i < n; ++i)
      std::cout << seq[i].id << '\n';

   return 0;
}