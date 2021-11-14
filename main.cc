#include "main.ih"

#include <cmath>
#include <iostream>
#include <numeric>
#include <tuple>

inline cppcoro::generator<LFT>
pi_lambert_lfts()
{
  return cppcoro::fmap(
    [](auto i) {
      return LFT{2 * i - 1, i * i, 1, 0};
    },
    Spigot::positive_integers<IntType>());
}

using LambertState = std::pair<LFT, IntType>;
inline cppcoro::generator<IntType>
pi_lambert()
{
  LambertState init = {{0, 4, 1, 0}, 1};
  auto lfts = cppcoro::fmap(
    [](auto i) {
      return LFT{2 * i - 1, i * i, 1, 0};
    },
    Spigot::positive_integers<IntType>());

  auto next = [](LambertState const& in)
  {
    auto&& [lft, index] = in;
    auto x = 2 * index - 1;
    return Rational{(lft.q * x + lft.r), (lft.s * x + lft.t)}.floor();
  };

  auto safe = [](LambertState const& in, IntType const& n)
  {
    auto&& [lft, index] = in;
    auto x = 5 * index - 2;
    return n ==
           Rational{(lft.q * x + 2 * lft.r), (lft.s * x + 2 * lft.t)}.floor();
  };
  auto prod = [](LambertState const& in, IntType const& n)
  {
    auto&& [z, i] = in;
    return std::make_pair((LFT{10, -10 * n, 0, 1}).compose(z), i);
  };
  auto cons = [](LambertState const& in, LFT const& z2)
  {
    auto&& [z, i] = in;
    auto new_z = z.compose(z2);
    if (i % 1024 == 0)
    {
      new_z = new_z.normalize();
    }
    return std::make_pair(new_z, IntType(i + 1));
  };

  return stream(*next, *safe, *prod, *cons, init, std::move(lfts));
}

int
main()
{
  auto count = 0;
  std::cout.setf(std::ios::unitbuf); // so we see individual digits appear

  for (auto digit : pi_leibniz())
  {
    std::cout << digit;
    // printf("%d", (int)digit);

    if (count > 10000)
    {
      break;
    }
    ++count;
  }
}
