#include "generator.hh"
#include <iostream>

struct LFT
{
  double q;
  double r;
  double s;
  double t;
};

double extr(LFT lft, double x)
{
  return (lft.q * x + lft.r) / (lft.s * x + lft.t);
}

LFT unit() { return {1, 0, 0, 1}; }

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
  return {q * u + r * w, q * v + r * x, s * u + t * w,
          s * v + t * x};
}

template <typename B, typename C>
using NextFun = C (*)(B);
template <typename B, typename C>
using SafeFun = bool (*)(B, C);
template <typename B, typename C>
using ProdFun = B (*)(B, C);
template <typename B, typename A>
using ConsFun = B (*)(A, B);

template <typename A, typename B, typename C>
cppcoro::generator<C> stream(NextFun<B, C> next, SafeFun<B, C> safe,
                             ProdFun<B, C> prod, ConsFun<B, A> cons,
                             B z, cppcoro::generator<A> xs)
{
  auto it = xs.begin();
  auto x = *it;

  while (true)
  {
    auto y = next(z);
    if (safe(z, y))
    {
      co_yield y;
      z = prod(z, y);
    }
    else
    {
      z = cons(z, x);
      ++it;
      x = *it;
    }
  }
}

#include <cmath>

cppcoro::generator<double> positive_doubleegers()
{
  double num = 1;
  while (true)
  {
    co_yield num;
    ++num;
  }
}

cppcoro::generator<LFT> pi_lfts()
{
  return cppcoro::fmap(
      [](auto k) {
        return LFT{k, 4 * k + 2, 0, 2 * k + 1};
      },
      positive_doubleegers());
}

cppcoro::generator<double> pi()
{
  auto init = unit();
  auto lfts = pi_lfts();

  auto next = [](LFT z) { return std::floor(extr(z, 3)); };
  auto safe
      = [](LFT z, double n) { return n == std::floor(extr(z, 4)); };
  auto prod = [](LFT z, double n) {
    return comp({10, -10 * n, 0, 1}, z);
  };
  auto cons = [](LFT z, LFT z2) { return comp(z, z2); };
  return stream(*next, *safe, *prod, *cons, init, std::move(lfts));
}

int main()
{
  auto count = 0;

  for (auto digit : pi())
  {
    std::cerr << digit;

    if (count > 1000)
    {
      break;
    }
    ++count;
  }
}
