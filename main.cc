#include "generator.hh"
#include "lft/lft.hh"
#include "rational/rational.hh"

#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>
#include <iostream>
#include <numeric>

using namespace boost::multiprecision;
using IntType = cpp_int;

using Rational = Spigot::Rational<IntType>;
using LFT = Spigot::LFT<IntType>;

template <typename State, typename Result>
using NextFun = Result (*)(State const &);
template <typename State, typename Result>
using SafeFun = bool (*)(State const &, Result const &);
template <typename State, typename Result>
using ProdFun = State (*)(State const &, Result const &);
template <typename State, typename Input>
using ConsFun = State (*)(State const &, Input const &);

template <typename Input, typename State, typename Result>
cppcoro::generator<Result>
stream(NextFun<State, Result> next, SafeFun<State, Result> safe,
       ProdFun<State, Result> prod, ConsFun<State, Input> cons,
       State z, cppcoro::generator<Input> elems)
{
  auto xs = elems.begin();
  Input &x = *xs;

  while (true)
  {
    Result y = next(z);
    if (safe(z, y))
    {
      z = prod(z, y);
      co_yield y;
    }
    else
    {
      z = cons(z, x);
      ++xs;
      x = *xs;
    }
  }
}

#include <cmath>

inline cppcoro::generator<IntType> positive_integers()
{
  IntType num = 1;
  while (true)
  {
    co_yield num;
    ++num;
  }
}

inline cppcoro::generator<LFT> pi_leibniz_lfts()
{
  return cppcoro::fmap(
      [](auto k) {
        return LFT{k, 4 * k + 2, 0, 2 * k + 1};
      },
      positive_integers());
}

inline cppcoro::generator<IntType> pi_leibniz()
{
  auto init = LFT::unit();
  auto lfts = pi_leibniz_lfts();

  auto next = [](LFT const &z) { return z(3).floor(); };
  auto safe = [](LFT const &z, IntType const &n)
  { return n == z(4).floor(); };
  auto prod = [](LFT const &z, IntType const &n) {
    return LFT{10, -10 * n, 0, 1}.compose(z);
  };
  auto cons
      = [](LFT const &z, LFT const &z2) { return z.compose(z2); };
  return stream(*next, *safe, *prod, *cons, init, std::move(lfts));
}

#include <tuple>

inline cppcoro::generator<LFT> pi_lambert_lfts()
{
  return cppcoro::fmap(
      [](auto i) {
        return LFT{2 * i - 1, i * i, 1, 0};
      },
      positive_integers());
}

using LambertPair = std::pair<LFT, IntType>;
inline cppcoro::generator<IntType> pi_lambert()
{
  LambertPair init = {{0, 4, 1, 0}, 1};
  auto lfts = pi_lambert_lfts();

  NextFun<LambertPair, IntType> next = [](LambertPair const &in)
  {
    auto &&[lft, index] = in;
    auto x = 2 * index - 1;
    return Rational{(lft.q * x + lft.r), (lft.s * x + lft.t)}
        .floor();
  };

  SafeFun<LambertPair, IntType> safe
      = [](LambertPair const &in, IntType const &n)
  {
    auto &&[lft, index] = in;
    auto x = 5 * index - 2;
    return n
           == Rational{(lft.q * x + 2 * lft.r),
                       (lft.s * x + 2 * lft.t)}
                  .floor();
  };
  ProdFun<LambertPair, IntType> prod
      = [](LambertPair const &in, IntType const &n)
  {
    auto &&[z, i] = in;
    return std::make_pair((LFT{10, -10 * n, 0, 1}).compose(z), i);
  };
  ConsFun<LambertPair, LFT> cons
      = [](LambertPair const &in, LFT const &z2)
  {
    auto &&[z, i] = in;
    auto new_z = z.compose(z2);
    if (i % 1024 == 0)
    {
      new_z = new_z.normalize();
    }
    return std::make_pair(new_z, IntType(i + 1));
  };

  return stream(next, safe, prod, cons, init, std::move(lfts));
}

int main()
{
  auto count = 0;
  // std::cout.setf(
  //     std::ios::unitbuf); // so we see individual digits appear

  for (auto digit : pi_lambert())
  {
    // std::cout << digit;
    printf("%d", (int)digit);

    if (count > 10000)
    {
      break;
    }
    ++count;
  }
}
