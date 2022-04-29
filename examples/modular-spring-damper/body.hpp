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

#include "ascent/ascent.hpp"

struct body_t final
{
   template <class state_t>
   body_t(state_t& state) : s(state), v(state) {}
   
   asc::param_t<double> s; // position
   asc::param_t<double> v; // velocity
   double m{}; // mass
   double f{}; // force
   
   template <class state_t>
   void operator()(const state_t&, state_t& D, const double)
   {
      s(D) = v;

      if (m > 0.0)
         v(D) = f / m;
      else
         v(D) = 0.0;

      f = 0.0;
   }
};
