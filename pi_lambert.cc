#include "main.ih"
#include <tuple>

namespace
{
  using LambertState = std::pair<LFT, IntType>;

  LambertState initialState() { return {{0, 4, 1, 0}, 1}; }

  Spigot::Generator<LFT> inputStream()
  {
    auto ints = Spigot::positive_integers<IntType>();
    for (auto const &num : ints)
    {
      co_yield LFT{2 * num - 1, num * num, 1, 0};
    }
  }

  IntType nextResult(LambertState const &state)
  {
    auto const &[lft, index] = state;
    auto const &[q, r, s, t] = lft;
    auto x = 2 * index - 1;
    return Rational{q * x + r, s * x + t}.floor();
  }

  bool safeToCommit(LambertState const &in, IntType const &n)
  {
    auto const &[lft, index] = in;
    auto const &[q, r, s, t] = lft;
    auto x = 5 * index - 2;
    return n == Rational{q * x + 2 * r, s * x + 2 * t}.floor();
  };

  LambertState extractProducedResult(
    LambertState const &in,
    IntType const &n)
  {
    auto const &[lft, index] = in;
    auto new_lft = LFT{10, -10 * n, 0, 1}.compose(lft);
    return {new_lft, index};
  }

  LambertState consumeInput(
    LambertState const &state,
    LFT const &input)
  {
    auto const &[lft, index] = state;
    auto new_lft = lft.compose(input);
    // if (index % 1024 == 0)
    // {
    //   new_trans = new_trans.normalize();
    // }
    return {new_lft, index + 1};
  }
}

Spigot::Generator<IntType>
pi_lambert()
{
  return stream(
    nextResult,
    safeToCommit,
    extractProducedResult,
    consumeInput,
    initialState(),
    inputStream());
}
