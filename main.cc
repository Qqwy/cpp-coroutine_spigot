#include "generator.hh"
#include "lft/lft.hh"
#include "rational/rational.hh"
#include "spigot/spigot.hh"

#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>
#include <iostream>
#include <numeric>
#include <tuple>

using namespace boost::multiprecision;
using IntType = cpp_int;

using Rational = Spigot::Rational<IntType>;
using LFT = Spigot::LFT<IntType>;

template<typename State, typename Result>
using NextFun = Result (*)(State const&);
template<typename State, typename Result>
using SafeFun = bool (*)(State const&, Result const&);
template<typename State, typename Result>
using ProdFun = State (*)(State const&, Result const&);
template<typename State, typename Input>
using ConsFun = State (*)(State const&, Input const&);

template<typename Input, typename State, typename Result>
cppcoro::generator<Result>
stream(
  NextFun<State, Result> nextResult,
  SafeFun<State, Result> safeToCommit,
  ProdFun<State, Result> extractProducedResult,
  ConsFun<State, Input> consumeInput,
  State const& initialState,
  cppcoro::generator<Input> input_stream)
{
  auto state = initialState;
  auto input_stream_iterator = input_stream.begin();
  Input& input = *input_stream_iterator;

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

struct PiLeibniz : public Spigot::Spigot<LFT, LFT, IntType>
{
  inline IntType next(LFT const& z) final override { return z(3).floor(); };

  inline bool safe(LFT const& z, IntType const& n) final override
  {
    return n == z(4).floor();
  }

  inline LFT prod(LFT const& z, IntType const& n) final override
  {
    return LFT{10, -10 * n, 0, 1}.compose(z);
  }

  inline LFT cons(LFT const& z, LFT const& z2) final override
  {
    return z.compose(z2);
  }

  inline cppcoro::generator<LFT> elems() final override
  {
    return cppcoro::fmap(
      [](auto k) {
        return LFT{k, 4 * k + 2, 0, 2 * k + 1};
      },
      ::Spigot::positive_integers<IntType>());
  }

  inline LFT initialState() override { return LFT::unit(); }
};

inline cppcoro::generator<IntType>
pi_leibniz()
{
  auto initial_state = LFT::unit();
  auto input_stream = cppcoro::fmap(
    [](auto k) {
      return LFT{k, 4 * k + 2, 0, 2 * k + 1};
    },
    Spigot::positive_integers<IntType>());

  auto nextState = [](LFT const& z) { return z(3).floor(); };
  auto safeToCommit = [](LFT const& z, IntType const& n)
  { return n == z(4).floor(); };
  auto extractProducedResult = [](LFT const& z, IntType const& n) {
    return LFT{10, -10 * n, 0, 1}.compose(z);
  };
  auto consumeInput = [](LFT const& z, LFT const& z2) { return z.compose(z2); };
  return stream(
    *nextState,
    *safeToCommit,
    *extractProducedResult,
    *consumeInput,
    initial_state,
    std::move(input_stream));
}

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

  // auto spigot = PiLeibniz{};
  // for (auto digit : spigot.stream())
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
