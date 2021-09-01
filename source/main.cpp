#include "ntc-sampling-point-calculator.h"

#include <fmt/format.h>

int main()
{
    constexpr auto ntcPoints = ntcSamplingPointCalculator<COUNT, true>();
    for (const auto& t : ntcPoints)
    {
        fmt::print("{0}\n", t);
    }
}
