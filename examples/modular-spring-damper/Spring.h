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

#include "Body.h"

struct Spring
{
   Spring(Body& b0, Body& b1) : b0(b0), b1(b1)
   {
      l0 = b1.s - b0.s;
   }

   Body& b0;
   Body& b1;

   double l0{}; // initial spring length (distance between masses)
   double ds{}; // spring compression/extension
   double k{}; // spring coefficient
   double f{}; // force

   void operator()(const asc::state_t&, asc::state_t&, const double)
   {
      ds = l0 + b0.s - b1.s;
      f = k*ds;

      b0.f -= f;
      b1.f += f;
   }
};