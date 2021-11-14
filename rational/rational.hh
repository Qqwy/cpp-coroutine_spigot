#ifndef RATIONAL_H
#define RATIONAL_H

namespace Spigot
{
/// A simple rational datatype
/// representing the rational number `num / denom`.
template<typename Integral>
struct Rational
{
  /// The numerator
  Integral num;

  /// The denominator
  Integral denom;

  /// Returns the integer part of the rational value
  inline Integral floor() { return num / denom; }

  /// Returns a rational with the same meaning,
  /// but in normal form
  ///
  /// Main advantages:
  /// - smaller numbers used for `num` and `denom`.
  /// - allows for fast comparison of rationals
  inline Rational normalize() const
  {
    Integral res = gcd(num, denom);
    return Rational{num / res, denom.res};
  }
};
} // namespace Spigot

#endif
