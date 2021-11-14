#include "generator.hh"
#include <iostream>

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

int main() { std::cout << "Boom\n"; }
