#ifndef SPIGOT_H
#define SPIGOT_H

#include "../generator.hh"

namespace Spigot {
  template<typename Input, typename State, typename Result>
  struct Spigot {
    virtual ~Spigot(){};

    virtual inline Result next(State const &) = 0;
    virtual inline bool safe(State const &, Result const &) = 0;
    virtual inline State prod(State const &, Result const &) = 0;
    virtual inline State cons(State const &, Input const &) = 0;
    virtual inline cppcoro::generator<Input> elems() = 0;
    virtual inline State initialState() = 0;

    inline cppcoro::generator<Result> stream();
  };

  template<typename Input, typename State, typename Result>
  cppcoro::generator<Result> Spigot<Input, State, Result>::stream()
  {
    auto state = initialState();
    auto gen = elems();
    auto xs = gen.begin();
    Input& x = *xs;

    while (true)
    {
      Result y = next(state);
      if (safe(state, y))
      {
        state = prod(state, y);
        co_yield y;
      }
      else
      {
        state = cons(state, x);
        ++xs;
        x = *xs;
      }
    }
  }

  template<typename Integral>
  inline cppcoro::generator<Integral> positive_integers()
  {
    Integral num = 1;
    while (true)
    {
      co_yield num;
      ++num;
    }
  }
}


#endif
