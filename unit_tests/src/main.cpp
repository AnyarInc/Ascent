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
#include "ascent/integrators_modular/ABM4.h"
#include "ascent/integrators_modular/VABM.h"
#include "ascent/timing/Timing.h"

#include <memory>

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
   void operator()(const state_t& x, state_t& xd, const double)
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

template <class Integrator>
std::pair<double, double> exponential_test_mod_adaptive()
{
   Integrator integrator;
   auto system = std::make_shared<ExponentialMod>();
   auto sim = std::make_shared<asc::Timing<double>>();
   auto settings = AdaptiveT<double>();
   system->value = 1.0;
   sim->t = 0.0;
   sim->t_end = 10.0;
   double dt = 0.001;
   settings.abs_tol = 1e-16;
   settings.rel_tol = 1e-16;
   std::vector<asc::Module *> blocks{};
   blocks.emplace_back(system.get());

   system->init();

   while (sim->t < sim->t_end)
   {
      integrator(blocks, sim->t, dt, settings);
   }

   return{ system->value, sim->t };
}

#include <boost/ut.hpp>

using namespace boost::ut;

inline bool approx(const auto& x, const auto near, const double epsilon = 1.0e-8) {
   return (x <= (near + epsilon)) && (x >= (near - epsilon));
}

suite airy_system = []
{
   "airy_rk4"_test = [] {
      auto x = airy_test<RK4>(0.001);
      boost::ut::
      expect(approx(x[0], -0.200693641142)) << x[0];
      expect(approx(x[1], -1.49817601143)) << x[1];
   };
   
   "airy_rk2"_test = [] {
      auto x = airy_test<RK2>(0.001);
      expect(approx(x[0], -0.200703911717)) << x[0];
      expect(approx(x[1], -1.49816435475)) << x[1];
   };
   
   "airy_dopri45"_test = [] {
      auto x = airy_test<DOPRI45>(0.001);
      expect(approx(x[0], -0.200693641142)) << x[0];
      expect(approx(x[1], -1.49817601143)) << x[1];
   };
};

suite airy_modular = []
{
   "airy_modular_rk4"_test = [] {
      auto x = airy_test_mod<modular::RK4<double>>(0.001);
      expect(approx(x[0], -0.200693641142)) << x[0];
      expect(approx(x[1], -1.49817601143)) << x[1];
   };
   
   "airy_modular_rk2"_test = [] {
      auto x = airy_test_mod<modular::RK2<double>>(0.001);
      expect(approx(x[0], -0.200703911717)) << x[0];
      expect(approx(x[1], -1.49816435475)) << x[1];
   };
   
   "airy_modular_pc233"_test = [] {
      auto x = airy_test_mod<modular::PC233<double>>(0.001);
      expect(approx(x[0], -0.200693641142)) << x[0];
      expect(approx(x[1], -1.49817601143)) << x[1];
   };
};

suite exponential = []
{
   "exp_rk4"_test = [] {
      auto result = exponential_test<RK4>(0.001);
      expect(approx(result.first[0], std::exp(result.second)));
   };
   
   "exp_rk2"_test = [] {
      auto result = exponential_test<RK2>(0.001);
      expect(approx(result.first[0], std::exp(result.second), 1.0e-1));
   };
   
   "exp_dopri45"_test = [] {
      auto result = exponential_test<DOPRI45>(0.001);
      expect(approx(result.first[0], std::exp(result.second)));
   };
   
   "exp_pc233"_test = [] {
      auto result = exponential_test<PC233>(0.001);
      expect(approx(result.first[0], std::exp(result.second), 1.0e-2));
   };
   
   "exp_abm4"_test = [] {
      auto result = exponential_test<ABM4>(0.001);
      expect(approx(result.first[0], std::exp(result.second)));
   };
};

suite exp_modular = []
{
   "exp_modular_rk4"_test = [] {
      auto result = exponential_test_mod<modular::RK4<double>>(0.001);
      expect(approx(result.first, std::exp(result.second)));
   };
   
   "exp_modular_rk2"_test = [] {
      auto result = exponential_test_mod<modular::RK2<double>>(0.001);
      expect(approx(result.first, std::exp(result.second), 1.0e-1));
   };
   
   "exp_modular_pc233"_test = [] {
      auto result = exponential_test_mod<modular::PC233<double>>(0.001);
      expect(approx(result.first, std::exp(result.second), 1.0e-2));
   };
   
   "exp_modular_abm4"_test = [] {
      auto result = exponential_test_mod<modular::ABM4<double>>(0.001);
      expect(approx(result.first, std::exp(result.second)));
   };
   
   "exp_modular_vabm"_test = [] {
      auto result = exponential_test_mod<modular::VABM<double>>(0.001);
      expect(approx(result.first, std::exp(result.second)));
   };
   
   "exp_modular_adaptive_vabm"_test = [] {
      auto result = exponential_test_mod_adaptive<modular::VABM<double>>();
      expect(approx(result.first, std::exp(result.second)));
   };
};

int main() {}
