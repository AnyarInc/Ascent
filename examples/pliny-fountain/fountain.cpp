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

// Pliny's Fountain example from:
// Applied Numerical Methods with MATLAB
// for Engineers and Scientists
// Steven C. Chapra
// p.608-609

struct Fountain
{
   double siphon;
   static constexpr double Rt = 0.05;
   static constexpr double r = 0.007;
   static constexpr double yhi = 0.1;
   static constexpr double ylo = 0.025;
   static constexpr double C = 0.6;
   static constexpr double g = 9.81;
   static constexpr double Qin = 0.00005;

   static constexpr double pi = 3.14159265359;

   void operator()(const state_t& x, state_t& xd, const double)
   {
      if (x[0] <= ylo)
         siphon = 0.0;
      else if (x[0] >= yhi)
         siphon = 1.0;

      const double Qout = siphon * C * sqrt(2 * g * x[0]) * pi * r * r;
      xd[0] = (Qin - Qout) / (pi * Rt * Rt);
   }
};

int main()
{
   state_t x = { 0.0 };
   double t = 0.0;
   double dt = 1.0;
   double t_end = 100.0;

   RK4 integrator;
   Fountain system;

   Recorder recorder;

   while (t < t_end)
   {
      recorder({ t, x[0] });
      integrator(system, x, t, dt);
   }

   recorder.csv("results", { "t", "x[0]" }); // generate a file of comma separated values

   return 0;
}