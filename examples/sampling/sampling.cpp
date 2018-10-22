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

#include "ascent/Ascent.h"

using namespace asc;

struct ODE
{
   void operator()(const state_t& x, state_t& xd, const double t)
   {
      xd[0] = cos(t);
   }
};

int main()
{
   state_t x = { 0.0 };
   double t = 0.0;
   double dt = 0.1;
   double t_end = 10.0;

   RK4 integrator;
   ODE system;

   Recorder recorder;

   while (t < t_end)
   {
      Sampler sampler(t, dt);

      // We force the system to be evaluated at all increments of 0.33 and 0.41, as well as trigger a single event evaluation at 0.617
      if (sampler(0.33) || sampler(0.41) || sampler.event(0.617))
         recorder({ t, x[0] });

      integrator(system, x, t, dt);
   }

   recorder.csv("sampling", { "t", "x0" }); // generate a file of comma separated values

   return 0;
}