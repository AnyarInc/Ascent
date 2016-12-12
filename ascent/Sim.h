// Copyright (c) 2016 Anyar, Inc.
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

#include "ascent/timing/Clock.h"

// A Sim (Simulator) is a fancy Clock.
// The Sim class isn't necessary, but it helps write common looping code and combines the concepts of a state Vector and a Clock.

namespace asc
{
   template <typename C>
   struct SimT : public ClockT<typename C::value_type>
   {
      C x;

      operator C&() { return x; }

      template <typename Functor>
      void run(Functor&& functor)
      {
         while (t < t_end)
            functor();
      }

      template <typename System, typename Integrator>
      void run(System&& system, Integrator&& integrator)
      {
         while (t < t_end)
            integrator(*this, x, system);
      }

      template <typename System, typename Integrator, typename Recorder>
      void run(System&& system, Integrator&& integrator, Recorder&& recorder)
      {
         while (t < t_end)
         {
            recorder();
            integrator(*this, x, system);
         }
      }
   };
}