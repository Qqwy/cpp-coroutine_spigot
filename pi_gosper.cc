#include "main.ih"
#include <tuple>

namespace
{
  using GosperState = std::pair<LFT, IntType>;
  GosperState initialState() { return {LFT::unit(), 1}; }

  cppcoro::generator<LFT> inputStream()
  {

    auto ints = Spigot::positive_integers<IntType>();
    for (auto const& num : ints)
    {
      auto num2 = 3 * (3 * num + 1) * (3 * num + 2);
      co_yield LFT{
        num * (2 * num - 1), num2 * (5 * num - 2), 0, num2};
    }
  }

  IntType nextResult(GosperState const& state)
  {
    auto const& [lft, index] = state;
    auto const& [q, r, s, t] = lft;
    auto x = 27 * index + 15;

    return (q * x + 5 * r) / (s * x + 5 * t);
  }

  bool safeToCommit(
    GosperState const& state,
    IntType const& n)
  {
    auto const& [lft, index] = state;
    auto const& [q, r, s, t] = lft;
    auto x = 675 * index - 216;
    return n == ((q * x + 125 * r) / (s * x + 125 * t));
  }

  GosperState extractProducedResult(
    GosperState const& state,
    IntType const& n)
  {
    auto const& [lft, index] = state;
    auto new_lft = LFT{10, -10 * n, 0, 1}.compose(lft);
    return {new_lft, index};
  }

  GosperState consumeInput(
    GosperState const& state,
    LFT const& input)
  {
    auto const& [lft, index] = state;
    auto new_lft = lft.compose(input);
    return {new_lft, index + 1};
  }
}

cppcoro::generator<IntType>
pi_gosper()
{
  return stream(
    nextResult,
    safeToCommit,
    extractProducedResult,
    consumeInput,
    initialState(),
    inputStream());
}
