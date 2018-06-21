// Copyright (c) 2016-2018 Anyar, Inc.
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

#include <functional>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace asc
{
   template <typename T>
   std::string to_string_precision(const T input, const int n = 6)
   {
      std::ostringstream out;
      out << std::setprecision(n) << input;
      return out.str();
   }
   
   /// Recorder is a fast, type specific class for generically recording data.
   ///
   /// The Recorder class permits variable width rows of data that can be populated by passing in initializer lists (preferred) or std::vectors
   /// \paramt T The type to be recorded.
   template <typename T>
   struct RecorderT
   {
      std::vector<std::function<std::vector<T>()>> recording_functions;
      std::vector<std::string> titles;
      std::vector<std::vector<T>> history;
      int precision{};

      inline void operator()(std::initializer_list<T>&& initializer) { history.emplace_back(std::move(initializer)); }
      inline void push_back(const std::vector<T>& v) { history.push_back(v); }

      inline void add(const T& x) { history.back().push_back(x); }
      inline void add(const std::vector<T>& v)
      {
         for (auto& x : v)
            history.back().push_back(x);
      }

      void add_title(const std::string& title)
      {
         titles.emplace_back(title);
      }

      void add_titles(const std::vector<std::string>& new_titles)
      {
         for (auto& title : new_titles)
            titles.emplace_back(title);
      }

      template <typename V>
      void record(V& x) { record(x, ""); }

      template <typename V>
      void record(V& x, const std::string& title)
      {
         titles.emplace_back(title);
         recording_functions.emplace_back([&]() -> std::vector<T> { return{ static_cast<T>(x) }; });
      }
      
      void record(std::vector<T>& v)
      {
         const size_t n = v.size();
         record(v, std::vector<std::string>(n));
      }
      void record(std::vector<T>& v, const std::vector<std::string>& new_titles)
      {
         for (auto& title : new_titles)
            titles.emplace_back(title);
         recording_functions.emplace_back([&]() -> std::vector<T> { return v; });
      }

      // Reserve memory for history vector
      void reserve(const size_t n) { history.reserve(n); }

      void update()
      {
         history.emplace_back(); // need to allocate the slot that we will be adding (appending) to
         for (auto& rec : recording_functions)
            add(rec());
      }

      /// Write out a csv file
      void csv(const std::string& file_name) const { csv(file_name, titles); }
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
                  if (precision > 0)
                     file << std::setprecision(precision) << history[i][j];
                  else
                     file << history[i][j]; // faster, but can truncate values
                  if (j < num_states - 1)
                     file << ", ";
               }
               file << '\n';
            }
         }
         else
            throw std::runtime_error("Record: file '" + file_name + ".csv' could not be opened.");
      }
   };
}
