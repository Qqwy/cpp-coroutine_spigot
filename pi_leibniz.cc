#include "main.ih"

namespace
{
  cppcoro::generator<LFT> inputStream()
  {
    auto ints = Spigot::positive_integers<IntType>();
    for(auto num : ints) {
      co_yield LFT{num, 4 * num + 2, 0, 2 * num + 1};
    }
  }

  auto nextState(LFT const& z) -> decltype(auto)
  {
    return z(3).floor();
  }

  auto safeToCommit(LFT const& z, IntType const& n)
    -> decltype(auto)
  {
    return n == z(4).floor();
  }

  auto extractProducedResult(LFT const& z, IntType const& n)
    -> decltype(auto)
  {
    return LFT{10, -10 * n, 0, 1}.compose(z);
  };

  auto consumeInput(LFT const& z, LFT const& z2)
    -> decltype(auto)
  {
    return z.compose(z2);
  }
}

cppcoro::generator<IntType>
pi_leibniz()
{
  return Spigot::stream(
    nextState,
    safeToCommit,
    extractProducedResult,
    consumeInput,
    LFT::unit(),
    inputStream());
}
