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

#include "ascent/integrators_modular/RK2.h"
#include "ascent/integrators_modular/RK4.h"
#include "ascent/integrators_modular/PC233.h"
#include "ascent/timing/Timing.h"

#include <iostream>

using namespace asc;

bool test(const std::string& title, const double x, const double target, const double eps)
{
   if (std::abs(x - target) > eps)
   {
      std::cout << title << " failed: " << to_string_precision(x, 12) << ", " << to_string_precision(target, 12) << '\n';
      return false;
   }
   std::cout << title << " results: " << to_string_precision(x, 12) << ", " << to_string_precision(target, 12) << '\n';
   return true;
}

struct TestResult {
   TestResult& operator = (const bool &input) {
      if (this->passed)
         this->passed = input;
      return *this;
   }
   bool failed() {
      return !passed;
   }
private:
   bool passed = true;
};

struct Airy
{
   void operator()(const state_t& x, state_t& xd, const double t)
   {
      xd[0] = x[1];
      xd[1] = -t*x[0];
   }
};

struct AiryMod : asc::Module
{
   double a{};
   double a_d{};
   double b{};
   double b_d{};
   std::shared_ptr<asc::Timing<double>> sim{};

   void init()
   {
      make_state(a, a_d);
      make_state(b, b_d);
   }

   void operator()()
   {
      a_d = b;
      b_d = -sim->t*a; 
   }
};

struct Exponential
{
   void operator()(const state_t& x, state_t& xd, const double t)
   {
      xd[0] = x[0];
   }
}; 
struct ExponentialMod : asc::Module
{
   double value{};
   double deriv{};

   void init()
   {
      make_state(value, deriv);
   }
   void operator()()
   {
      deriv = value;
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
std::vector<double> airy_test_mod(const double dt)
{
   Integrator integrator;
   auto system = std::make_shared<AiryMod>();
   system->sim = std::make_shared<asc::Timing<double>>(); 
   system->a = 1.0;
   system->b = 0.0;
   system->sim->t = 0.0;
   system->sim->t_end = 10.0;
   std::vector<asc::Module*> blocks{};
   blocks.emplace_back(system.get());

   system->init();

   while (system->sim->t < system->sim->t_end)
   {
      integrator(blocks, system->sim->t, dt);
   }

   return{ system->a, system->b };
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
template <class Integrator>
std::pair<double, double> exponential_test_mod(const double dt)
{
   Integrator integrator;
   auto system = std::make_shared<ExponentialMod>();
   auto sim = std::make_shared<asc::Timing<double>>();
   system->value = 1.0;
   sim->t = 0.0;
   sim->t_end = 10.0;
   std::vector<asc::Module*> blocks{};
   blocks.emplace_back(system.get());

   system->init();

   while (sim->t < sim->t_end)
   {
      integrator(blocks, sim->t, dt);
   }

   return{ system->value, sim->t };
}

int main()
{
   TestResult res;

   auto x = airy_test<RK4>(0.001);
   res = test("Airy RK4 x[0]", x[0], -0.200693641142, 1.0e-8);
   res = test("Airy RK4 x[1]", x[1], -1.49817601143, 1.0e-8);
   x = airy_test<RK2>(0.001);
   res = test("Airy RK2 x[0]", x[0], -0.200703911717, 1.0e-8);
   res = test("Airy RK2 x[1]", x[1], -1.49816435475, 1.0e-8);
   x = airy_test<DOPRI45>(0.001);
   res = test("Airy DOPRI45 x[0]", x[0], -0.200693641142, 1.0e-8); // compared to RK4 results
   res = test("Airy DOPRI45 x[1]", x[1], -1.49817601143, 1.0e-8); // compared to RK4 results
   
   std::cout << "\n";
   auto x2 = airy_test_mod<modular::RK4<double>>(0.001);
   res = test("Airy RK4 Modular a", x2[0], -0.200693641142, 1.0e-8);
   res = test("Airy RK4 Modular b", x2[1], -1.49817601143, 1.0e-8);
   x2 = airy_test_mod<modular::RK2<double>>(0.001);
   res = test("Airy RK2 Modular a", x2[0], -0.200703911717, 1.0e-8);
   res = test("Airy RK2 Modular b", x2[1], -1.49816435475, 1.0e-8);
   x2 = airy_test_mod<modular::PC233<double>>(0.001);
   res = test("Airy PC233 Modular a", x2[0], -0.200693641142, 1.0e-8);
   res = test("Airy PC233 Modular b", x2[1], -1.49817601143, 1.0e-8);

   std::cout << "\n";
   auto result = exponential_test<RK4>(0.001);
   res = test("Exponential RK4 x", result.first[0], exp(result.second), 1.0e-8);
   result = exponential_test<RK2>(0.001);
   res = test("Exponential RK2 x", result.first[0], exp(result.second), 1.0e-1);
   result = exponential_test<DOPRI45>(0.001);
   res = test("Exponential DOPRI45 x", result.first[0], exp(result.second), 1.0e-8);
   result = exponential_test<PC233>(0.001);
   res = test("Exponential PC233 x", result.first[0], exp(result.second), 1.0e-2);

   std::cout << "\n";
   auto result2 = exponential_test_mod<modular::RK4<double>>(0.001);
   res = test("Exponential RK4 Modular x", result2.first, exp(result2.second), 1.0e-8);
   result2 = exponential_test_mod<modular::RK2<double>>(0.001);
   res = test("Exponential RK2 Modular x", result2.first, exp(result2.second), 1.0e-1);
   result2 = exponential_test_mod<modular::PC233<double>>(0.001);
   res = test("Exponential PC233 Modular x", result2.first, exp(result2.second), 1.0e-2);

   return res.failed();
}