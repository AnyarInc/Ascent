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

#include "ascent/Ascent.h"

#include <iostream>

using namespace asc;

bool test(const std::string& title, const double x, const double target, const double eps)
{
   if (std::abs(x - target) > eps)
   {
      std::cout << title << " failed: " << to_string_precision(x, 12) << ", " << to_string_precision(target, 12) << '\n';
      return false;
   }
   return true;
}

struct Airy
{
   void operator()(const state_t& x, state_t& xd, const double t)
   {
      xd[0] = x[1];
      xd[1] = -t*x[0];
   }
};

struct Exponential
{
   void operator()(const state_t& x, state_t& xd, const double t)
   {
      xd[0] = x[0];
   }
};

template <class Integrator>
state_t airy_test(const double dt)
{
   state_t x = { 1.0, 0.0 };
   double t = 0.0;
   double t_end = 10.0;

   Integrator integrator;
   Airy system;

   while (t < t_end)
   {
      integrator(system, x, t, dt);
   }

   return x;
}

template <class Integrator>
std::pair<state_t, double> exponential_test(const double dt)
{
   state_t x = { 1.0 };
   double t = 0.0;
   double t_end = 10.0;

   Integrator integrator;
   Exponential system;

   while (t < t_end)
   {
      integrator(system, x, t, dt);
   }

   return{ x, t };
}

int main()
{
   auto x = airy_test<RK4>(0.001);
   test("Airy RK4 x[0]", x[0], -0.200693641142, 1.0e-8);
   test("Airy RK4 x[1]", x[1], -1.49817601143, 1.0e-8);
   x = airy_test<RK2>(0.001);
   test("Airy RK2 x[0]", x[0], -0.200703911717, 1.0e-8);
   test("Airy RK2 x[1]", x[1], -1.49816435475, 1.0e-8);
   x = airy_test<DOPRI45>(0.001);
   test("Airy DOPRI45 x[0]", x[0], -0.200693641142, 1.0e-8); // compared to RK4 results
   test("Airy DOPRI45 x[1]", x[1], -1.49817601143, 1.0e-8); // compared to RK4 results

   auto result = exponential_test<RK4>(0.001);
   test("Exponential RK4 x", result.first[0], exp(result.second), 1.0e-8);
   result = exponential_test<RK2>(0.001);
   test("Exponential RK2 x", result.first[0], exp(result.second), 1.0e-1);
   result = exponential_test<DOPRI45>(0.001);
   test("Exponential DOPRI45 x", result.first[0], exp(result.second), 1.0e-8);
   result = exponential_test<PC233>(0.001);
   test("Exponential PC233 x", result.first[0], exp(result.second), 1.0e-2);

   return 0;
}