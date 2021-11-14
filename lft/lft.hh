#ifndef LFT_H
#define LFT_H

#include "../rational/rational.hh"

namespace Spigot
{
  /// A Linear Fractional Transformation
  /// also known as a Möbius Transformation
  ///
  /// Can be interpreted as the transformation matrix
  ///  | q  r |
  ///  | s  t |
  ///
  /// and as the function taking x -> (q * x + r ) / (s * x + t).
  template<typename Integral>
  struct LFT
  {
    // NOTE: We use SLVs here
    // because all literature uses these names
    Integral q;
    Integral r;
    Integral s;
    Integral t;

    static inline LFT unit() { return {1, 0, 0, 1}; }

    inline Rational<Integral> operator()(Integral x) const
    {
      return {(q * x + r), (s * x + t)};
    }

    inline LFT normalize() const
    {
      auto common_factor = gcd(gcd(q, r), gcd(s, t));
      return {
        q / common_factor,
        r / common_factor,
        s / common_factor,
        t / common_factor};
    }

    inline LFT compose(LFT other) const
    {
      auto const& [u, v, w, x] = other;
      return {q * u + r * w, q * v + r * x, s * u + t * w, s * v + t * x};
    }
  };
} // namespace Spigot

#endif
