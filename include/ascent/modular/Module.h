// Copyright (c) 2016-2019 Anyar, Inc.
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

#include "ascent/direct/State.h"

#include <memory>
#include <vector>

namespace asc
{
   struct Module
   {
      Module() = default;
      Module(const Module&) = default;
      Module(Module&&) = default;
      Module& operator=(const Module&) = default;
      Module& operator=(Module&&) = default;
      virtual ~Module() = default;

      std::vector<State> states;

      template <class x_t, class xd_t>
      void make_state(x_t& x, xd_t& xd)
      {
         states.emplace_back(x, xd);
      }

      template <class x_t, class xd_t>
      void make_states(x_t& x, xd_t& xd)
      {
         const size_t n = x.size();
         for (size_t i = 0; i < n; ++i)
         {
            states.emplace_back(x[i], xd[i]);
         }
      }

      template <class states_t>
      void add_states(states_t& ext_states)
      {
         for (auto& state : states)
         {
            ext_states.emplace_back(state);
         }
      }

      virtual void init() {} // initialization
      virtual void operator()() {} // derivative updates
      virtual void postcalc() {} // post integration calculations

      bool init_called = false;
   };

   template <class modules_t>
   inline void init(modules_t& modules)
   {
      for (auto& module : modules)
      {
         if (!module->init_called)
         {
            module->init();
            module->init_called = true;
         }   
      }
   }

   template <class modules_t>
   inline void update(modules_t& modules)
   {
      for (auto& module : modules)
      {
         module->operator()();
      }
   }

   template <class modules_t>
   inline void postcalc(modules_t& modules)
   {
      for (auto& module : modules)
      {
         module->postcalc();
      }
   }

   template <class states_t>
   inline void add_states(states_t& states, std::vector<std::shared_ptr<Module>>& modules)
   {
      for (auto& module : modules)
      {
         auto& m_states = module->states;
         for (auto& state : m_states)
         {
            states.emplace_back(state);
         }
      }
   }

   template <class states_t>
   inline void add_states(states_t& states, Module& module)
   {
      for (auto& state : module.states)
      {
         states.emplace_back(state);
      }
   }

   template <class states_t>
   inline void add_states(states_t& states, std::shared_ptr<Module>& module)
   {
      for (auto& state : module->states)
      {
         states.emplace_back(state);
      }
   }
}