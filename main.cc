#include "main.ih"

#include <cmath>
#include <iostream>
#include <numeric>
#include <tuple>

int main()
{
  auto count = 0;
  std::cout.setf(std::ios::unitbuf); // so we see individual
                                     // digits appear

  for (auto digit : pi_lambert())
  {
    std::cout << digit.ulong();
    // printf("%d", (int)digit);

    if (count > 10000)
    {
      break;
    }
    ++count;
  }
}
