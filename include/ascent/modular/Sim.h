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

#include "ascent/modular/Module.h"

#include <memory>

namespace asc
{
   struct Sim : Module
   {
      std::vector<std::shared_ptr<Module>> modules;

      template <class module_t, class... Args>
      auto emplace_back(Args&&... args)
      {
         modules.emplace_back(std::make_shared<module_t>(args...));
         return std::static_pointer_cast<module_t>(modules.back());
      }

      void push_back(std::shared_ptr<Module>&& module)
      {
         modules.emplace_back(module);
      }

      void init(std::vector<State>& states) override
      {
         for (auto& module : modules)
         {
            module->init(states);
         }
      }

      void operator()() override
      {
         for (auto& module : modules)
         {
            module->operator()();
         }
      }

      void postcalc() override
      {
         for (auto& module : modules)
         {
            module->postcalc();
         }
      }
   };
}