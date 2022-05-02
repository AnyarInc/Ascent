// Developed by Anyar, Inc.
// Distributed under 0BSD (see LICENSE for details)

#include "ascent/integrators/rk4.hpp"
#include "ascent/recorder.hpp"

#include <vector>

using namespace asc;

using state_t = std::vector<double>;

struct lorenz_t
{
   void operator()(const state_t& x, state_t& xd, const double) const noexcept
   {
      static constexpr double sigma = 10.0;
      static constexpr double R = 28.0;
      static constexpr double b = 8.0 / 3.0;

      xd[0] = sigma * (x[1] - x[0]);
      xd[1] = R * x[0] - x[1] - x[0] * x[2];
      xd[2] = -b * x[2] + x[0] * x[1];
   }
};

int main()
{
   state_t x = { 10.0, 1.0, 1.0 };
   double t = 0.0;
   double dt = 0.01;
   double t_end = 10.0;

   rk4_t<state_t> integrator;
   lorenz_t system;

   recorder_t<double> recorder;

   while (t < t_end)
   {
      recorder({ t, x[0], x[1], x[2] });
      integrator(system, x, t, dt);
   }

   recorder.csv("lorenz", { "t", "x0", "x1", "x2" });

   return 0;
}
