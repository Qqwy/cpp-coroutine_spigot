#include "main.ih"

using namespace boost::multiprecision;
using IntType = cpp_int;

using Rational = Spigot::Rational<IntType>;
using LFT = Spigot::LFT<IntType>;

namespace
{
  cppcoro::generator<LFT> inputStream()
  {
    return cppcoro::fmap(
      [](auto k) {
        return LFT{k, 4 * k + 2, 0, 2 * k + 1};
      },
      Spigot::positive_integers<IntType>());
  }

  auto nextState(LFT const& z) -> decltype(auto) { return z(3).floor(); }

  auto safeToCommit(LFT const& z, IntType const& n) -> decltype(auto)
  {
    return n == z(4).floor();
  }

  auto extractProducedResult(LFT const& z, IntType const& n) -> decltype(auto)
  {
    return LFT{10, -10 * n, 0, 1}.compose(z);
  };

  auto consumeInput(LFT const& z, LFT const& z2) -> decltype(auto)
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
