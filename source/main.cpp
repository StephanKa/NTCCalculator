#include "ntc-sampling-point-calculator.h"

#include <fmt/format.h>

struct NTCConfig
{
    using Temperature = float;
    using Ohm = float;
    static constexpr Ohm RESISTANCE = {10000.0f};
    static constexpr float B_CONSTANT = {4100.0f};
    static constexpr Temperature REF_TEMPERATURE = {25.0f};
    static constexpr Temperature OFFSET = {273.15f};
    static constexpr bool PullDown = {true};
};

struct CircuitConfig
{
    using Temperature = float;
    using Ohm = float;
    static constexpr Temperature MIN_TEMPERATURE = -10.0f;
    static constexpr Temperature MAX_TEMPERATURE = 100.0f;
    static constexpr float FACTOR_SAMPLING_POINTS = 0.3f;
    static constexpr uint32_t COUNT = static_cast<uint32_t>((MAX_TEMPERATURE - MIN_TEMPERATURE + 1.0f) * FACTOR_SAMPLING_POINTS);
    static constexpr float SUPPLY_VOLTAGE = {3.3f};
    static constexpr Ohm PRE_RESISTANCE = {10000.0f};
};

constexpr uint8_t ADC_RESOLUTION = 12u;

int main()
{
    constexpr auto ntcPoints = ntcSamplingPointCalculator<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
    fmt::print("Count: {0}\n", CircuitConfig::COUNT);
    for (const auto& t : ntcPoints)
    {
        fmt::print("{0} - {1}mV\n", t, t * CircuitConfig::SUPPLY_VOLTAGE * 1000.0f / Math::pow(2, ADC_RESOLUTION));
    }
}
