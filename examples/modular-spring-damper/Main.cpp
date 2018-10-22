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

using namespace asc;

int main()
{
   state_t x;
   x.reserve(100); // We reserve more space than necessary, but Ascent will only allocate what is needed
   double t = 0.0;
   double dt = 0.01;
   double t_end = 1.5;

   Body b0(x);
   Body b1(x);
   b1.m = 1.0;
   b1.s = 1.0;
   b1.v = 40.0;

   Spring spring(b0, b1);
   spring.k = 2000.0;

   Damper damper(b0, b1);
   damper.c = 5.0;

   RK4 integrator;
   Recorder recorder;

   auto system = [&](const asc::state_t& x, asc::state_t& D, const double t)
   {
      // We must run the spring and damper before the body in order to accumulate forces
      spring(x, D, t);
      damper(x, D, t);
      b1(x, D, t);
   };

   while (t < t_end)
   {
      recorder({ t, b1.s });
      integrator(system, x, t, dt);
   }

   recorder.csv("spring-damper", { "t", "b1 position" });

   return 0;
}