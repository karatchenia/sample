#pragma once

#include <stdexcept>

//It is assumed that beginInclusive <= endInclusive,
// and the "divisor" is not zero.
template<typename Number>
inline Number divisors_in_range(
  const Number& beginInclusive, const Number& endInclusive,
  const Number& divisor)
{
  const auto result = endInclusive / divisor - (beginInclusive - 1) / divisor;
  return result;
}

//The "modulus" must be not zero.
template<typename Number>
Number modular_power(Number base, Number exponent, const Number modulus)
{
#ifdef _DEBUG
  if (0 == modulus)
  {
    throw std::out_of_range("The modulus must not be zero.");
  }
#endif

  base = base % modulus;

  Number result = 1;
  while (exponent)
  {
    if (exponent & 1)
    {//Particular bit is set in the "exponent".
      result = (result * base) % modulus;
    }

    base = (base*base) % modulus;
    exponent = exponent >> 1;
  }

  return result;
}

template<typename Number>
Number gcd(Number a, Number b)
{
  if (0 == a)
  {
    return b;
  }

  if (0 == b)
  {
    return a;
  }

  auto d = 0;

  while (0 == ((a | b) & 1))
  {//Both even.
    a >>= 1;
    b >>= 1;
    ++d;
  }

  while (a != b)
  {
    if (0 == (a & 1))
    {
      a >>= 1;
    }
    else if (0 == (b & 1))
    {
      b >>= 1;
    }
    else if (b < a)
    {
      a = (a - b) >> 1;
    }
    else
    {
      b = (b - a) >> 1;
    }
  }

  const Number result = a << d;
  return result;
}