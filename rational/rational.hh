#ifndef RATIONAL_H
#define RATIONAL_H

namespace Spigot
{
  /// A simple rational datatype
  /// representing the rational number `num / denom`.
  template <typename Integral>
  struct Rational
  {
    Integral num;
    Integral denom;

    inline Integral floor() { return num / denom; }
    inline Rational normalize(Rational in)
    {
      Integral res = gcd(in.num, in.denom);
      return Rational{in.num / res, in.denom.res};
    }
  };
} // namespace Spigot

#endif
