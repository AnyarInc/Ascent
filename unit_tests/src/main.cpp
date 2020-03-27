// Copyright (c) 2016-2020 Anyar, Inc.
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

using namespace asc;

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

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

// bool test(const std::string& title, const double x, const double target, const double eps)

TEST_CASE("Airy System RK4", "[airy]") {
   auto x = airy_test<RK4>(0.001);
   REQUIRE(x[0] == Approx(-0.200693641142).epsilon(1.0e-8));
   REQUIRE(x[1] == Approx(-1.49817601143).epsilon(1.0e-8));
}

TEST_CASE("Airy System RK2", "[airy]") {
   auto x = airy_test<RK2>(0.001);
   REQUIRE(x[0] == Approx(-0.200703911717).epsilon(1.0e-8));
   REQUIRE(x[1] == Approx(-1.49816435475).epsilon(1.0e-8));

   x = airy_test<DOPRI45>(0.001);
   REQUIRE(x[0] == Approx(-0.200693641142).epsilon(1.0e-8));
   REQUIRE(x[1] == Approx(-1.49817601143).epsilon(1.0e-8));
}

TEST_CASE("Airy System DOPRI45", "[airy]") {
   auto x = airy_test<DOPRI45>(0.001);
   REQUIRE(x[0] == Approx(-0.200693641142).epsilon(1.0e-8));
   REQUIRE(x[1] == Approx(-1.49817601143).epsilon(1.0e-8));
}

TEST_CASE("Modular Airy System RK4", "[airy][modular]") {
   auto x = airy_test_mod<modular::RK4<double>>(0.001);
   REQUIRE(x[0] == Approx(-0.200693641142).epsilon(1.0e-8));
   REQUIRE(x[1] == Approx(-1.49817601143).epsilon(1.0e-8));
}

TEST_CASE("Modular Airy System RK2", "[airy][modular]") {
   auto x = airy_test_mod<modular::RK2<double>>(0.001);
   REQUIRE(x[0] == Approx(-0.200703911717).epsilon(1.0e-8));
   REQUIRE(x[1] == Approx(-1.49816435475).epsilon(1.0e-8));
}

TEST_CASE("Modular Airy System PC233", "[airy][modular]") {
   auto x = airy_test_mod<modular::PC233<double>>(0.001);
   REQUIRE(x[0] == Approx(-0.200693641142).epsilon(1.0e-8));
   REQUIRE(x[1] == Approx(-1.49817601143).epsilon(1.0e-8));
}

TEST_CASE("Exponential RK4", "[exponential]") {
   auto result = exponential_test<RK4>(0.001);
   REQUIRE(result.first[0] == Approx(exp(result.second)).epsilon(1.0e-8));
}

TEST_CASE("Exponential RK2", "[exponential]") {
   auto result = exponential_test<RK2>(0.001);
   REQUIRE(result.first[0] == Approx(exp(result.second)).epsilon(1.0e-5));
}

TEST_CASE("Exponential DOPRI45", "[exponential]") {
   auto result = exponential_test<DOPRI45>(0.001);
   REQUIRE(result.first[0] == Approx(exp(result.second)).epsilon(1.0e-8));
}

TEST_CASE("Exponential PC233", "[exponential]") {
   auto result = exponential_test<PC233>(0.001);
   REQUIRE(result.first[0] == Approx(exp(result.second)).epsilon(1.0e-6));
}

TEST_CASE("Exponential Modular RK4", "[exponential][modular]") {
   auto result = exponential_test_mod<modular::RK4<double>>(0.001);
   REQUIRE(result.first == Approx(exp(result.second)).epsilon(1.0e-8));
}

TEST_CASE("Exponential Modular RK2", "[exponential][modular]") {
   auto result = exponential_test_mod<modular::RK2<double>>(0.001);
   REQUIRE(result.first == Approx(exp(result.second)).epsilon(1.0e-5));
}

TEST_CASE("Exponential Modular PC233", "[exponential][modular]") {
   auto result = exponential_test_mod<modular::PC233<double>>(0.001);
   REQUIRE(result.first == Approx(exp(result.second)).epsilon(1.0e-6));
}