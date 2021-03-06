#ifndef SPIGOT_H
#define SPIGOT_H

#include "../generator/generator.hh"

namespace Spigot
{
  // Feel free to swap this out
  // with another generator implementation
  // if you want to compare them
  template<typename Value>
  using Generator = cppcoro::generator<Value>;

  template<typename State, typename Result>
  using NextFun = Result (*)(State const &);

  template<typename State, typename Result>
  using SafeFun = bool (*)(State const &, Result const &);

  template<typename State, typename Result>
  using ProdFun = State (*)(State const &, Result const &);

  template<typename State, typename Input>
  using ConsFun = State (*)(State const &, Input const &);

  template<typename Input, typename State, typename Result>
  Generator<Result> stream(
    NextFun<State, Result> nextResult,
    SafeFun<State, Result> safeToCommit,
    ProdFun<State, Result> extractProducedResult,
    ConsFun<State, Input> consumeInput,
    State state,
    Generator<Input> input_stream)
  {
    auto input_stream_iterator = input_stream.begin();
    Input &input = *input_stream_iterator;

    while (true)
    {
      Result result = nextResult(state);
      if (safeToCommit(state, result))
      {
        state = extractProducedResult(state, result);
        co_yield result;
      }
      else
      {
        state = consumeInput(state, input);
        ++input_stream_iterator;
        input = *input_stream_iterator;
      }
    }
  }

  template<typename Integral>
  inline Generator<Integral> positive_integers()
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
