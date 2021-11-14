#ifndef SPIGOT_H
#define SPIGOT_H

#include "../generator.hh"

namespace Spigot {
  template<typename Input, typename State, typename Result>
  class Spigot {
    virtual ~Spigot(){};

    virtual inline Result next(State const &);
    virtual inline bool safe(State const &, Result const &);
    virtual inline State prod(State const &, Result const &);
    virtual inline State cons(State const &, Input const &);
    virtual inline cppcoro::generator<Input> elems();
    virtual inline State initialState();

    inline cppcoro::generator<Result> stream();
  };

  template<typename Input, typename State, typename Result>
  cppcoro::generator<Result> Spigot<Input, State, Result>::stream()
  {
    auto state = initialState();
    auto xs = elems().begin();
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
}


#endif
