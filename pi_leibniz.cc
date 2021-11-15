#include "main.ih"

namespace
{
  Spigot::Generator<LFT> inputStream()
  {
    auto ints = Spigot::positive_integers<IntType>();
    for (auto const &num : ints)
    {
      co_yield LFT{num, 4 * num + 2, 0, 2 * num + 1};
    }
  }

  auto nextState(LFT const &lft) -> decltype(auto)
  {
    return lft(3).floor();
  }

  auto safeToCommit(LFT const &lft, IntType const &digit) -> decltype(auto)
  {
    return digit == lft(4).floor();
  }

  auto extractProducedResult(LFT const &lft, IntType const &digit)
    -> decltype(auto)
  {
    return LFT{10, -10 * digit, 0, 1}.compose(lft);
  };

  auto consumeInput(LFT const &lft, LFT const &input) -> decltype(auto)
  {
    return lft.compose(input);
  }
}

Spigot::Generator<IntType> pi_leibniz()
{
  return Spigot::stream(
    nextState,
    safeToCommit,
    extractProducedResult,
    consumeInput,
    LFT::unit(),
    inputStream());
}
