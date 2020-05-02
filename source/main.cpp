#include "ntc-sampling-point-calculator.h"
#include <iostream>

int main()
{
    constexpr auto ntcPoints = ntcSamplingPointCalculator<COUNT, true>();
    for (auto& t : ntcPoints)
    {
        std::cout << t << "\n";
    }
}