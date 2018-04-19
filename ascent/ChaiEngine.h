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

#include "ascent/Ascent.h"
#include "chaiscript/chaiscript.hpp"
#include "chaiscript/chaiscript_stdlib.hpp"

namespace asc
{
   struct ChaiEngine : public chaiscript::ChaiScript
   {
      // Captures the systyem object by reference.
	   template <typename T>
	   void addSystem()
	   {
		   add(chaiscript::type_conversion<T&, system_t>([](T& system) -> system_t { return [&](const state_t& x, state_t& xd, const value_t t) { return system(x, xd, t); }; }));
         add(chaiscript::type_conversion<std::shared_ptr<T>, system_t>([](std::shared_ptr<T> system) -> system_t { return [=](const state_t& x, state_t& xd, const value_t t) { return (*system)(x, xd, t); }; }));
	   }

      // This code must exist here and not within the Recorder class for compatibilty with compilers that inspect templates (i.e. Xcode's LLVM)
      template <typename T, typename ChaiScript>
      void scriptRecorder(ChaiScript& c, const std::string& name)
      {
         using namespace chaiscript;
         using R = RecorderT<T>;
         c.add(user_type<R>(), name);
         c.add(constructor<R()>(), name);

         c.add(fun(&R::titles), "titles");
         c.add(fun(&R::reserve), "reserve");
         c.add(fun(&R::precision), "precision");
         c.add(fun(&R::update), "update");
         c.add(fun([](R& rec, const T& x) { rec.add(x); }), "add");
         c.add(fun([](R& rec, const std::vector<T>& data) { rec.add(data); }), "add");
         c.add(fun([](R& rec, const std::string& title) { rec.add_title(title); }), "add_title");
         c.add(fun([](R& rec, const std::vector<std::string>& titles) { rec.add_titles(titles); }), "add_titles");
         c.add(fun([](R& rec, T& x) { rec.record(x); }), "record");
         c.add(fun([](R& rec, T& x, const std::string& title) { rec.record(x, title); }), "record");
         c.add(fun([](R& rec, std::vector<T>& v) { rec.record(v); }), "record");
         c.add(fun([](R& rec, std::vector<T>& v, const std::vector<std::string>& title) { rec.record(v, title); }), "record");
         c.add(fun([](R& rec, const std::vector<T>& data) { rec.push_back(data); }), "push_back");
         c.add(fun([](const R& rec, const std::string& file_name) { rec.csv(file_name); }), "csv");
         c.add(fun([](const R& rec, const std::string& file_name, const std::vector<std::string>& names) { rec.csv(file_name, names); }), "csv");
      }

      ChaiEngine()
      {
         add(chaiscript::vector_conversion<state_t>());
         add(chaiscript::vector_conversion<std::vector<std::string>>());
         add(chaiscript::bootstrap::standard_library::vector_type<state_t>("state_t"));

         scriptRecorder<asc::value_t>(*this, "Recorder");
         add(chaiscript::fun([](RecorderT<asc::value_t>& rec, int& x) { rec.record(x); }), "record");
         add(chaiscript::fun([](RecorderT<asc::value_t>& rec, size_t& x) { rec.record(x); }), "record");
         add(chaiscript::fun([](RecorderT<asc::value_t>& rec, int& x, const std::string& title) { rec.record(x, title); }), "record");
         add(chaiscript::fun([](RecorderT<asc::value_t>& rec, size_t& x, const std::string& title) { rec.record(x, title); }), "record");

         // This allows for more generic mixing of various data types, all they need is to be converted to a string prior to being passed into the recorder
         // The RecorderString is primarily for data that is going to be output to a file and not operated on during the simulation
         scriptRecorder<std::string>(*this, "RecorderString");

         add(chaiscript::user_type<asc::Param>(), "Param");
         add(chaiscript::constructor<asc::Param(state_t&)>(), "Param");
         add(chaiscript::constructor<asc::Param(state_t&, const value_t)>(), "Param");
         add(chaiscript::fun([](asc::Param& param, const value_t y) { return param = y; }), "=");
         add(chaiscript::fun([](asc::Param& param) { std::cout << param << '\n'; }), "print");
         add(chaiscript::fun([](asc::Param& param) { return static_cast<value_t>(param); }), "value");

         add(chaiscript::user_type<ParamV>(), "ParamV");
         add(chaiscript::constructor<ParamV(state_t&, const size_t)>(), "ParamV");
         add(chaiscript::fun([](ParamV& lhs, const std::vector<asc::value_t>& rhs) { return lhs = rhs; }), "=");
         add(chaiscript::fun([](ParamV& v) { v.zero(); }), "zero");

         add(chaiscript::user_type<Sampler>(), "Sampler");
         add(chaiscript::constructor<Sampler(asc::value_t&, asc::value_t&)>(), "Sampler");
         add(chaiscript::fun(&Sampler::operator()), "eval");
         add(chaiscript::fun(&Sampler::event), "event");
         add(chaiscript::fun(&Sampler::reset), "reset");

         // System
         add(chaiscript::user_type<System>(), "System");
         add(chaiscript::constructor<System()>(), "System");
         add(chaiscript::fun([](System& sys, const asc::state_t& x, asc::state_t& D, const asc::value_t t) { sys(x, D, t); }), "eval");
         add(chaiscript::type_conversion<System&, asc::system_t>());
         add(chaiscript::fun([](System& sys, const system_t& func) { sys.push_back(func);  }), "push_back");

         // Integrators
         integrator<Euler>("ascEuler");
         integrator<RK2>("RK2");
         integrator<RK4>("RK4");
      }
      ChaiEngine(const ChaiEngine&) = default;
      ChaiEngine(ChaiEngine&&) = default;
      ChaiEngine& operator=(const ChaiEngine&) = default;
      ChaiEngine& operator=(ChaiEngine&&) = default;

   private:
      template <typename T>
      void integrator(const std::string& name)
      {
         add(chaiscript::user_type<T>(), name);
         add(chaiscript::constructor<T()>(), name);
         add(chaiscript::fun([](T& integrator, system_t& system, state_t& state, value_t& t, const value_t dt) { integrator(system, state, t, dt); }), "step");
      }

      template <typename T>
      void adaptiveIntegrator(const std::string& name)
      {
         add(chaiscript::user_type<T>(), name);
         add(chaiscript::constructor<T(const value_t epsilon)>(), name);
         add(chaiscript::fun([](T& integrator, system_t& system, state_t& state, value_t& t, value_t& dt) { integrator(system, state, t, dt); }), "step");
      }
   };
}
