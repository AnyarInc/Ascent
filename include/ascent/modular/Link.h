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
      std::shared_ptr<std::remove_cv_t<T>> module{}; // Non-const qualified module for doing low level, potentially dangerous simulation stuff. Used for asynchronous module access.

      template <class... Args>
      Link(Args&&... args) : module(std::make_shared<std::remove_cv_t<T>>(args...)) {}

      Link(const Link&) = default;
      Link(Link&&) = default;
      Link& operator=(const Link&) = default;
      Link& operator=(Link&&) = default;

      Link& operator=(const std::shared_ptr<T>& ptr)
      {
         module = ptr;
         return *this;
      }

      operator std::shared_ptr<T>()
      {
         return module;
      }

      operator std::shared_ptr<Module>()
      {
         return module;
      }

      // The pointer is copied with T qualifiers, so that the module can be const qualified.
      T* operator -> ()
      {
         if (!module)
         {
            throw std::runtime_error(("nullptr access ->: " + class_info()).c_str());
         }

         if (!module->init_called)
         {
            module->init();
            module->init_called = true;
         }

         return module.get();
      }

      explicit operator bool() const { return (module.get() != 0); }

      void reset() { module.reset(); }

      std::string class_info() const { return "Link<" + static_cast<std::string>(typeid(T).name()) + '>'; }
   };

   template <class T> bool operator==(const Link<T>& link, std::nullptr_t ptr) { return link.module == ptr; }
   template <class T> bool operator==(std::nullptr_t ptr, const Link<T>& link) { return link.module == ptr; }

   template <class T> bool operator!=(const Link<T>& link, std::nullptr_t ptr) { return link.module != ptr; }
   template <class T> bool operator!=(std::nullptr_t ptr, const Link<T>& link) { return link.module != ptr; }
}