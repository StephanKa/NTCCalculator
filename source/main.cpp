#include "NTCCalculator.h"

struct NTCConfig
{
    static constexpr Ohm RESISTANCE = {10000.0f};
    static constexpr float B_CONSTANT = {4100.0f};
    static constexpr Temperature REF_TEMPERATURE = {25.0f};
    static constexpr bool PullDown = {true};
};

struct CircuitConfig
{
    static constexpr Temperature MIN_TEMPERATURE = -10.0f;
    static constexpr Temperature MAX_TEMPERATURE = 100.0f;
    static constexpr uint32_t COUNT = 24;
    static constexpr Volt SUPPLY_VOLTAGE = {3.3f};
    static constexpr Ohm PRE_RESISTANCE = {10000.0f};
};

constexpr uint8_t ADC_RESOLUTION = 12u;

int main()
{
    constexpr auto ntcPoints = NTC::samplingPointCalculator<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
    fmt::print("Count: {0}\n", CircuitConfig::COUNT);
    for (const auto& t : ntcPoints)
    {
        fmt::print("{0} - {1}mV\n", t, static_cast<float>(t) * CircuitConfig::SUPPLY_VOLTAGE * 1000.0f / Math::pow(2, ADC_RESOLUTION));
    }

    // dump the circuit as ASCII
    NTC::Draw::dump<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
}
