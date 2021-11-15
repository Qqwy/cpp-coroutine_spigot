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

  IntType nextResult(LFT const &lft) { return lft(3).floor(); }

  bool safeToCommit(LFT const &lft, IntType const &digit)
  {
    return digit == lft(4).floor();
  }

  LFT extractProducedResult(LFT const &lft, IntType const &digit)
  {
    return LFT{10, -10 * digit, 0, 1}.compose(lft);
  };

  LFT consumeInput(LFT const &lft, LFT const &input)
  {
    return lft.compose(input);
  }
}

Spigot::Generator<IntType> pi_leibniz()
{
  return Spigot::stream(
    nextResult,
    safeToCommit,
    extractProducedResult,
    consumeInput,
    LFT::unit(),
    inputStream());
}
