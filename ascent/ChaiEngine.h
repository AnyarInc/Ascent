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
	   }

      ChaiEngine()
      {
         add(chaiscript::vector_conversion<state_t>());
         add(chaiscript::vector_conversion<std::vector<std::string>>());
         add(chaiscript::bootstrap::standard_library::vector_type<state_t>("state_t"));

         add(chaiscript::user_type<Recorder>(), "Recorder");
         add(chaiscript::constructor<Recorder()>(), "Recorder");
         add(chaiscript::fun(&Recorder::record), "record");
         add(chaiscript::fun(&Recorder::reserve), "reserve");
         add(chaiscript::fun([](Recorder& record, const state_t& data) { record.push_back(data); }), "push_back");
         add(chaiscript::fun([](const Recorder& recorder, const std::string& file_name) { recorder.csv(file_name); }), "csv");
         add(chaiscript::fun([](const Recorder& recorder, const std::string& file_name, const std::vector<std::string>& names) { recorder.csv(file_name, names); }), "csv");
         add(chaiscript::type_conversion<Recorder&, std::function<void()>>([](Recorder& recorder) -> std::function<void()> { return [&] { return recorder.record(); }; }));

         // This allows for more generic mixing of various data types, all they need is to be converted to a string prior to being passed into the recorder
         // The RecorderString is primarily for data that is going to be output to a file and not operated on during the simulation
         add(chaiscript::user_type<RecorderString>(), "RecorderString");
         add(chaiscript::constructor<RecorderString()>(), "RecorderString");
         add(chaiscript::fun(&RecorderString::record), "record");
         add(chaiscript::fun(&RecorderString::reserve), "reserve");
         add(chaiscript::fun([](RecorderString& record, const std::vector<std::string>& data) { record.push_back(data); }), "push_back");
         add(chaiscript::fun([](const RecorderString& recorder, const std::string& file_name) { recorder.csv(file_name); }), "csv");
         add(chaiscript::fun([](const RecorderString& recorder, const std::string& file_name, const std::vector<std::string>& names) { recorder.csv(file_name, names); }), "csv");
         add(chaiscript::type_conversion<RecorderString&, std::function<void()>>([](RecorderString& recorder) -> std::function<void()> { return [&] { return recorder.record(); }; }));

         add(chaiscript::user_type<asc::State>(), "State");
         add(chaiscript::constructor<asc::State(state_t&)>(), "State");
         add(chaiscript::constructor<asc::State(state_t&, const value_t)>(), "State");
         add(chaiscript::fun([](asc::State& state, const value_t y) { return state = y; }), "=");
         add(chaiscript::fun([](asc::State& state) { std::cout << state << '\n'; }), "print");
         add(chaiscript::fun([](asc::State& state) { return static_cast<value_t>(state); }), "value");

         add(chaiscript::user_type<StateVector>(), "StateVector");
         add(chaiscript::constructor<StateVector(state_t&, const size_t)>(), "StateVector");
         add(chaiscript::fun([](StateVector& lhs, const std::vector<asc::value_t>& rhs) { return lhs = rhs; }), "=");
         add(chaiscript::fun([](StateVector& v) { v.zero(); }), "zero");

         // Integrators
         integrator<Euler>("Euler");
         integrator<RK2>("RK2");
         integrator<RK4>("RK4");
         adaptiveIntegrator<RKMM>("RKMM");
      }

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