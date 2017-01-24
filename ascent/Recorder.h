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

#include <fstream>
#include <functional>
#include <vector>

// Recorder is a fast, type specific class for generically recording data
// The Recorder class permits variable width rows of data that can be populated by passing in initializer lists (preferred) or std::vectors

namespace asc
{
   template <typename T>
   struct RecorderT
   {
      inline void operator()(std::initializer_list<T>&& initializer) { history.emplace_back(std::move(initializer)); }
      inline void push_back(const std::vector<T>& x) { history.push_back(x); }

      void reserve(const size_t n) { history.reserve(n); }

      void csv(const std::string& file_name) const { csv(file_name, std::vector<std::string>()); }
      void csv(const std::string& file_name, const std::vector<std::string>& names) const
      {
         std::ofstream file;
         file.open(file_name + ".csv");

         if (file)
         {
            const size_t num_names = names.size();
            for (size_t i = 0; i < num_names; ++i)
            {
               file << names[i].c_str();
               if (i < num_names - 1)
                  file << ", ";
            }
            if (num_names > 0)
               file << '\n';

            const size_t num_steps = history.size();
            for (size_t i = 0; i < num_steps; ++i)
            {
               const size_t num_states = history.front().size();
               for (size_t j = 0; j < num_states; ++j)
               {
                  file << history[i][j];
                  if (j < num_states - 1)
                     file << ", ";
               }
               file << '\n';
            }
         }
         else
            throw std::runtime_error("Record: file '" + file_name + ".csv' could not be opened.");
      }

      std::function<void()> record; // a record function for ChaiScript
      std::vector<std::vector<T>> history;
   };
}