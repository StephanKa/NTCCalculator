#include "NTCCalculator.h"

using namespace unit_literals;
struct NTCConfig
{
    static constexpr auto RESISTANCE = 10000.0_Ohm;
    static constexpr float B_CONSTANT{4100.0f};
    static constexpr auto REF_TEMPERATURE = 25.0_Temp;
    static constexpr bool PULL_DOWN{true};
};

struct CircuitConfig
{
    static constexpr auto MIN_TEMPERATURE = Temperature(-10.0);
    static constexpr auto MAX_TEMPERATURE = 100.0_Temp;
    static constexpr uint32_t COUNT{24u};
    static constexpr auto SUPPLY_VOLTAGE = 3.3_Volt;
    static constexpr auto PRE_RESISTANCE = 10000.0_Ohm;
};

constexpr uint8_t ADC_RESOLUTION = 12u;
constexpr auto CONVERSION = [](auto value) { return static_cast<float>(value) * CircuitConfig::SUPPLY_VOLTAGE() * 1000.0f / Math::pow(2, ADC_RESOLUTION); };

int main()
{
    constexpr auto NTC_POINTS = NTC::samplingPointCalculator<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
    fmt::print("Count: {0}\n", CircuitConfig::COUNT);
    for (const auto& t : NTC_POINTS)
    {
        fmt::print("{0} - {1}mV\n", t, CONVERSION(t));
    }

    // dump the circuit as ASCII
    NTC::Draw::dump<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
    return 0;
}
