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

namespace asc
{
   namespace modular
   {
      template <class value_t>
      struct TimeStepper
      {
         TimeStepper() = default;
         TimeStepper(const TimeStepper&) = default;
         TimeStepper(TimeStepper&&) = default;
         TimeStepper& operator=(const TimeStepper&) = default;
         TimeStepper& operator=(TimeStepper&&) = default;
         virtual ~TimeStepper() {}

         virtual void operator()(const size_t, value_t&, const value_t) = 0; // inputs: pass, t (time), dt (time step)
      };
   }
}