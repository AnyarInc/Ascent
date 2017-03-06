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

/*
namespace blitz
{
template <typename C>
struct DOPRI87T
{
template <typename Clock, typename System>
void operator()(Clock& clock, C& x, System&& system)
{
auto& t = clock.t;
t0 = clock.t;
dt = clock.dt;
dt_2 = half*dt;
n = x.size();
if (xd0.size() < n)
{
xd0.resize(n);
xd1.resize(n);
xd2.resize(n);
xd3.resize(n);
}
for (size_t pass = 0; pass < 4; ++pass)
{
switch (pass)
{
case 0:
x0 = x;
system(x, xd0, t);
core::propagate(x, dt_2, xd0, x0);
t += dt_2;
break;
case 1:
system(x, xd1, t);
core::propagate(x, dt_2, xd1, x0);
break;
case 2:
system(x, xd2, t);
core::propagate(x, dt, xd2, x0);
t = t0 + dt;
break;
case 3:
system(x, xd3, t);
core::propRK4(x, dt, xd0, xd1, xd2, xd3, x0);
break;
default:
break;
}
}
}
private:
// Formula constants per pass
typename C::value_type c0 = static_cast<typename C::value_type>(1.0 / 18.0);
static constexpr std::array<typename C::value_type, 2> c1{ 1.0 / 48.0, 1.0 / 16.0 };
static constexpr std::array<typename C::value_type, 2> c2{ 1.0 / 32.0, 3.0 / 32.0 };
static constexpr std::array<typename C::value_type, 3> c3{ 5.0 / 16.0, -75.0 / 64.0, 75.0 / 64.0 };
static constexpr std::array<typename C::value_type, 3> c4{ 3.0 / 80.0, 3.0 / 16.0, 3.0 / 20.0 };
static constexpr std::array<typename C::value_type, 4> c5{ 29443841.0 / 614563906.0, 77736538.0 / 692538347.0, -28693883.0 / 1125000000.0, 23124283.0 / 1800000000.0 };
static constexpr std::array<typename C::value_type, 5> c6{ 16016141.0 / 946692911.0, 61564180.0 / 158732637.0, 22789713.0 / 633445777.0, 545815736.0 / 2771057229.0, -180193667.0 / 1043307555.0 };
static constexpr std::array<typename C::value_type, 6> c7{ 39632708.0 / 573591083.0, -433636366.0 / 683701615.0, -421739975.0 / 2616292301.0, 100302831.0 / 723423059.0, 790204164.0 / 839813087.0, 800635310.0 / 3783071287.0 };
static constexpr std::array<typename C::value_type, 7> c8{ 246121993.0 / 1340847787.0, -37695042795.0 / 15268766246.0, -309121744.0 / 1061227803.0, -12992083.0 / 490766935.0, 6005943493.0 / 2108947869.0, 393006217.0 / 1396673457.0, 123872331.0 / 1001029789.0 };
static constexpr std::array<typename C::value_type, 8> c9{ -1028468189.0 / 846180014.0, 8478235783.0 / 508512852.0, 1311729495.0 / 1432422823.0, -10304129995.0 / 1701304382.0, -48777925059.0 / 3047939560.0, 15336726248.0 / 1032824649.0, -45442868181.0 / 3398467696.0, 3065993473.0 / 597172653.0 };
static constexpr std::array<typename C::value_type, 9> c10{ 185892177.0 / 718116043.0, -3185094517.0 / 667107341.0, -477755414.0 / 1098053517.0, -703635378.0 / 230739211.0, 5731566787.0 / 1027545527.0, 5232866602.0 / 850066563.0, -4093664535.0 / 808688257.0, 3962137247.0 / 1805957418.0, 65686358.0 / 487910083.0 };
static constexpr std::array<typename C::value_type, 9> c11{ 403863854.0 / 491063109.0, -5068492393.0 / 434740067.0, -411421997.0 / 543043805.0, 652783627.0 / 914296604.0, 11173962825.0 / 925320556.0, -13158990841.0 / 6184727034.0, 3936647629.0 / 1978049680.0, -160528059.0 / 685178525.0, 248638103.0 / 1413531060.0 };
static constexpr std::array<typename C::value_type, 9> c12{ 14005451.0 / 335480064.0, -59238493.0 / 1068277825.0, 181606767.0 / 758867731.0, 561292985.0 / 797845732.0, -1041891430.0 / 1371343529.0, 760417239.0 / 1151165299.0, 118820643.0 / 751138087.0, -528747749.0 / 2220607170.0, 1.0 / 4.0 };
static constexpr std::array<typename C::value_type, 10> dt_ratios{ 1.0 / 18.0, 1.0 / 12.0, 1.0 / 8.0, 5.0 / 16.0, 3.0 / 8.0, 59.0 / 400.0, 93.0 / 200.0, 5490023248.0 / 9719169821.0, 13.0 / 20.0, 1201146811.0 / 1299019798.0 };
size_t n;
typename C::value_type t0;
C x0, xd0, xd1, xd2, xd3, xd4, xd5, xd6, xd7, xd8, xd9, xd10, xd11, xd12;
};
}*/