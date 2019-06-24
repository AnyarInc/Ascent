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

#include "ascent/containers/stack.h"

namespace asc
{
   template <class T>
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
   template <class T, size_t block_size = 4096>
   struct RecorderT
   {
      RecorderT() = default;
      RecorderT(const RecorderT&) = default;
      RecorderT(RecorderT&&) = default;
      RecorderT& operator=(const RecorderT&) = default;
      RecorderT& operator=(RecorderT&&) = default;

      std::vector<std::function<std::vector<T>()>> recording_functions; //!< std::vector of type erasing converters (std::functions) to enable user defined classes to be recorded.
      std::vector<std::string> titles; //!< Titles that will appear at the top row of a CSV export.
      asc::stack<std::vector<T>, block_size> history; //!< All recorded data is stored in history.
      int precision{}; //!< An integer to specify the recording precision for CSV output. If precision is not set (left at 0) then values will be exported with the c++ filestream default precision. This precision only affects export to CSV.

      /// \brief Add row of history data
      ///
      /// \param[in] initializer_list An input initializer list. Highly efficient as it performs a move operation on the data list.
      inline void operator()(std::initializer_list<T>&& initializer_list) { history.emplace_back(std::move(initializer_list)); }

      /// \brief Add row of history data
      ///
      /// \param[in] v A std::vector of data to be saved.
      inline void push_back(const std::vector<T>& v) { history.emplace_back(v); }

      /// \brief Add an item to the current row of data
      ///
      /// \param[in] x Item to be added to last row of history.
      inline void add(const T& x) { history.back().push_back(x); }

      /// \brief Add a std::vector of items to the current row of data
      ///
      /// \param[in] v A std::vector of items to be added to last row of history.
      inline void add(const std::vector<T>& v)
      {
         for (auto& x : v)
            history.back().push_back(x);
      }

      /// \brief Add a title for the next column of data for output to CSV.
      ///
      /// \param[in] title The title name for a column of data, used in the first row of the CSV generation.
      void add_title(const std::string& title)
      {
         titles.emplace_back(title);
      }

      /// \brief Add a std::vector of titles for the next columns of data for output to CSV.
      ///
      /// \param[in] new_titles A std::vector of title names, used in the first row of the CSV generation.
      void add_titles(const std::vector<std::string>& new_titles)
      {
         for (auto& title : new_titles)
            titles.emplace_back(title);
      }

      /// \brief Wrap a user defined type that has an implicit or explicit conversion to this Recorder's type T.
      ///
      /// \param[in] x The user defined input type, internally a reference to this instance is saved, so the Recorder should not outlive the object.
      template <typename V>
      void record(V& x) { record(x, ""); }

      /// \brief Wrap a user defined type that has an implicit or explicit conversion to this Recorder's type T.
      ///
      /// \param[in] x The user defined input type, internally a reference to this instance is saved, so the Recorder should not outlive the object.
      /// \param[in] title The title associated with this input variable.
      template <typename V>
      void record(V& x, const std::string& title)
      {
         titles.emplace_back(title);
         recording_functions.emplace_back([&]() -> std::vector<T> { return{ static_cast<T>(x) }; });
      }

      /// \brief Specify a std::vector<T> of this recorder's type T, to be saved every time update is called.
      ///
      /// \param[in] v Reference vector to be recorded upon update.
      void record(std::vector<T>& v)
      {
         const size_t n = v.size();
         record(v, std::vector<std::string>(n));
      }

      /// \brief Specify a std::vector<T> of this recorder's type T, to be saved every time update is called.
      ///
      /// \param[in] v Reference vector to be recorded upon update.
      /// \param[in] new_titles Specify the title for each component of the vector that will be recorded.
      void record(std::vector<T>& v, const std::vector<std::string>& new_titles)
      {
         for (auto& title : new_titles)
            titles.emplace_back(title);
         recording_functions.emplace_back([&]() -> std::vector<T> { return v; });
      }

      /// \brief All variables specified via the record functions will be saved when update is called.
      void update()
      {
         history.emplace_back(); // need to allocate the slot that we will be adding (appending) to
         for (auto& rec : recording_functions)
            add(rec());
      }

      /// \brief Write out a Comma Separated Value (CSV) file from the recorded data.
      ///
      /// \param[in] file_name The path and name of the file to be generated, excepting the .csv which is added by the function.
      void csv(const std::string& file_name) { csv(file_name, titles); }

      /// \brief Write out a Comma Separated Value (CSV) file from the recorded data.
      ///
      /// \param[in] file_name The path and name of the file to be generated, excepting the .csv which is added by the function.
      /// \param[in] names The variable names associated with each column of data.
      void csv(const std::string& file_name, const std::vector<std::string>& names)
      {
         std::ofstream file;
         file.open(file_name + ".csv");

         if (file)
         {
            if (precision > 0)
            {
               file.precision(precision);
            }

            const size_t num_names = names.size();
            for (size_t i = 0; i < num_names; ++i)
            {
               file << names[i].c_str();
               if (i < num_names - 1)
               {
                  file << ",";
               }
            }
            if (num_names > 0)
            {
               file << '\n';
            }

            const size_t num_states = history.front().size();

            for (auto& step : history)
            {
               for (auto i = 0; i < num_states; ++i)
               {
                  file << step[i];
                  if (i < num_states - 1)
                  {
                     file << ",";
                  }
               }
               file << '\n';
            }
         }
         else
            throw std::runtime_error("Record: file '" + file_name + ".csv' could not be opened.");
      }
   };
}
