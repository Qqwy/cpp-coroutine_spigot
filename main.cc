#include "generator.hh"
#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>
#include <iostream>
#include <numeric>

using namespace boost::multiprecision;
using IntType = cpp_int;

struct Rational
{
  IntType num;
  IntType denom;
};

Rational normalize(Rational in)
{
  IntType res = gcd(in.num, in.denom);
  return {in.num / res, in.denom / res};
}

IntType floor(Rational in) { return in.num / in.denom; }

struct LFT
{
  IntType q;
  IntType r;
  IntType s;
  IntType t;
};

Rational extr(LFT lft, IntType x)
{
  return {(lft.q * x + lft.r), (lft.s * x + lft.t)};
}

LFT unit() { return {1, 0, 0, 1}; }

LFT normalize(LFT in)
{
  IntType a = gcd(in.q, in.r);
  IntType b = gcd(in.s, in.t);
  IntType res = gcd(a, b);

  return {in.q / res, in.r / res, in.s / res, in.t / res};
}

LFT comp(LFT a, LFT b)
{
  auto q = a.q;
  auto r = a.r;
  auto s = a.s;
  auto t = a.t;

  auto u = b.q;
  auto v = b.r;
  auto w = b.s;
  auto x = b.t;
  // return normalize({q * u + r * w, q * v + r * x, s * u + t * w,
  //     s * v + t * x});

  return {q * u + r * w, q * v + r * x, s * u + t * w,
          s * v + t * x};
}

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

cppcoro::generator<IntType> positive_integers()
{
  IntType num = 1;
  while (true)
  {
    co_yield num;
    ++num;
  }
}

cppcoro::generator<LFT> pi_leibniz_lfts()
{
  return cppcoro::fmap(
      [](auto k) {
        return LFT{k, 4 * k + 2, 0, 2 * k + 1};
      },
      positive_integers());
}

cppcoro::generator<IntType> pi_leibniz()
{
  auto init = unit();
  auto lfts = pi_leibniz_lfts();

  auto next = [](LFT const &z) { return floor(extr(z, 3)); };
  auto safe = [](LFT const &z, IntType const &n)
  { return n == floor(extr(z, 4)); };
  auto prod = [](LFT const &z, IntType const &n) {
    return comp({10, -10 * n, 0, 1}, z);
  };
  auto cons
      = [](LFT const &z, LFT const &z2) { return comp(z, z2); };
  return stream(*next, *safe, *prod, *cons, init, std::move(lfts));
}

#include <tuple>

cppcoro::generator<LFT> pi_lambert_lfts()
{
  return cppcoro::fmap(
      [](auto i) {
        return LFT{2 * i - 1, i * i, 1, 0};
      },
      positive_integers());
}

using LambertPair = std::pair<LFT, IntType>;
cppcoro::generator<IntType> pi_lambert()
{
  LambertPair init = {{0, 4, 1, 0}, 1};
  auto lfts = pi_lambert_lfts();

  NextFun<LambertPair, IntType> next = [](LambertPair const &in)
  {
    auto &&[lft, index] = in;
    auto x = 2 * index - 1;
    return floor(Rational{(lft.q * x + lft.r), (lft.s * x + lft.t)});
  };

  SafeFun<LambertPair, IntType> safe
      = [](LambertPair const &in, IntType const &n)
  {
    auto &&[lft, index] = in;
    auto x = 5 * index - 2;
    return n
           == floor(Rational{(lft.q * x + 2 * lft.r),
                             (lft.s * x + 2 * lft.t)});
  };
  ProdFun<LambertPair, IntType> prod
      = [](LambertPair const &in, IntType const &n)
  {
    auto &&[z, i] = in;
    return std::make_pair(comp({10, -10 * n, 0, 1}, z), i);
  };
  ConsFun<LambertPair, LFT> cons
      = [](LambertPair const &in, LFT const &z2)
  {
    auto &&[z, i] = in;
    auto new_z = comp(z, z2);
    if (i % 1024 == 0)
    {
      new_z = normalize(new_z);
    }
    return std::make_pair(new_z, IntType(i + 1));
  };

  return stream(*next, *safe, *prod, *cons, init, std::move(lfts));
}

int main()
{
  auto count = 0;
  std::cout.setf(
      std::ios::unitbuf); // so we see individual digits appear

  for (auto digit : pi_lambert())
  {
    std::cout << digit;

    if (count > 10000)
    {
      break;
    }
    ++count;
  }
}
