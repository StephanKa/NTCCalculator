#include "ntc-sampling-point-calculator.h"

#include <fmt/format.h>

struct NTCConfig
{
    static constexpr float MIN_TEMPERATURE = -10.0f;
    static constexpr float MAX_TEMPERATURE = 100.0f;
    static constexpr float FACTOR_SAMPLING_POINTS = 0.2f;
    static constexpr uint32_t COUNT = static_cast<uint32_t>((MAX_TEMPERATURE - MIN_TEMPERATURE + 1.0f) * FACTOR_SAMPLING_POINTS);
    static constexpr float SUPPLY_VOLTAGE = {3.3f};
    static constexpr float RESISTANCE = {10000.0f};
    static constexpr float B_CONSTANT = {4100.0f};
    static constexpr float REF_TEMPERATURE = {25.0f};
    static constexpr float OFFSET = {273.15f};
    static constexpr bool PullDown = {true};
};

constexpr uint8_t ADC_RESOLUTION = 12u;

int main()
{
    constexpr auto ntcPoints = ntcSamplingPointCalculator<NTCConfig, ADC_RESOLUTION>();
    for (const auto& t : ntcPoints)
    {
        fmt::print("{0}\n", t);
    }
}
