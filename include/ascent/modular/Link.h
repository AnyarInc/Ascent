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
   template <class T>
   struct Link
   {
      std::remove_cv_t<T>>* module{}; // Non-const qualified module for doing low level, potentially dangerous simulation stuff.
                                      // The pointer will be returned with T qualifiers, so that the module can be const qualified.

      Link& operator=(const T* ptr) noexcept
      {
         module = ptr;
         return *this;
      }

      T& operator*()
      {
         check();
         return *module;
      }
      
      T* operator->()
      {
         check();
         return module;
      }

      explicit operator bool() const noexcept { return (module != 0); }

      std::string to_string() const { return "Link<" + static_cast<std::string>(typeid(T).name()) + '>'; }

   private:
      void check_init()
      {
         if (!module->init_run)
         {
            if (module->init_called)
            {
               throw std::runtime_error("Circular dependency for init(): " + to_string());
            }
            else
            {
               module->init_called = true;
               module->init();
            }
            module->init_run = true;
         }
      }

      void check()
      {
         if (module)
         {
            assert(module->phase);
            // The Link phase and Postprop phase do not check initialization.
            // Linking may occur prior to initialization and is not ordered.
            // Postprop is not sequenced, as calculations may only be performed on propagated states
            switch (*module->phase)
            {
            case Phase::Init:
               check_init();
               return;
            case Phase::Update:
               check_init();
               if (!module->update_run)
               {
                  if (module->update_called)
                  {
                     throw std::runtime_error("Circular dependency for update operator(): " + to_string());
                  }
                  else
                  {
                     module->update_called = true;
                     module->operator()();
                  }
                  module->update_run = true;
               }
               return;
            case Phase::Postcalc:
               check_init();
               return;
            case default:
               return;
            }
         }
         else
         {
            throw std::runtime_error(("nullptr access ->: " + to_string()).c_str());
         }
      }
   };

   template <class T> bool operator==(const Link<T>& link, std::nullptr_t ptr) noexcept { return link.module == ptr; }
   template <class T> bool operator==(std::nullptr_t ptr, const Link<T>& link) noexcept { return link.module == ptr; }

   template <class T> bool operator!=(const Link<T>& link, std::nullptr_t ptr) noexcept { return link.module != ptr; }
   template <class T> bool operator!=(std::nullptr_t ptr, const Link<T>& link) noexcept { return link.module != ptr; }
}