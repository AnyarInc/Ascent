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

struct Lorenz
{
   void operator()(const state_t& x, state_t& xd, const double)
   {
      static constexpr double sigma = 10.0;
      static constexpr double R = 28.0;
      static constexpr double b = 8.0 / 3.0;

      xd[0] = sigma * (x[1] - x[0]);
      xd[1] = R * x[0] - x[1] - x[0] * x[2];
      xd[2] = -b * x[2] + x[0] * x[1];
   }
};

int main()
{
   state_t x = { 10.0, 1.0, 1.0 };
   double t = 0.0;
   double dt = 0.01;
   double t_end = 10.0;

   RK4 integrator;
   Lorenz system;

   Recorder recorder;

   while (t < t_end)
   {
      recorder({ t, x[0], x[1], x[2] });
      integrator(system, x, t, dt);
   }

   recorder.csv("lorenz", { "t", "x0", "x1", "x2" });

   return 0;
}