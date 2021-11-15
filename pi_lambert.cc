#include "main.ih"
#include <tuple>

namespace
{
  using LambertState = std::pair<LFT, IntType>;

  LambertState initialState() { return std::make_pair<LFT, IntType>({0, 4, 1, 0}, 1); }

  cppcoro::generator<LFT> inputStream()
  {
    return cppcoro::fmap(
      [](auto num) {
        return LFT{2 * num - 1, num * num, 1, 0};
      },
      Spigot::positive_integers<IntType>());
  }

  IntType nextResult(LambertState const& in)
  {
    auto&& [lft, index] = in;
    auto x = 2 * index - 1;
    return Rational{
      (lft.q * x + lft.r), (lft.s * x + lft.t)}
      .floor();
  }

  bool safeToCommit(
    LambertState const& in,
    IntType const& n)
  {
    auto&& [lft, index] = in;
    auto x = 5 * index - 2;
    return n ==
           Rational{
             (lft.q * x + 2 * lft.r),
             (lft.s * x + 2 * lft.t)}
             .floor();
  };

  LambertState extractProducedResult(
    LambertState const& in,
    IntType const& n)
  {
    auto const &[trans, index] = in;
    return std::make_pair<LFT, IntType>((LFT{10, -10 * n, 0, 1}).compose(trans), IntType{index});
  }

  LambertState consumeInput(
    LambertState const& state,
    LFT const& input)
  {
    auto&& [trans, index] = state;
    auto new_trans = trans.compose(input);
    if (index % 1024 == 0)
    {
      new_trans = new_trans.normalize();
    }
    return std::make_pair(new_trans, IntType(index + 1));
  }
}

cppcoro::generator<IntType>
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
