#include "NTCCalculator.h"

struct NTCConfig
{
    static constexpr auto RESISTANCE = Ohm(10000.0f);
    static constexpr float B_CONSTANT{4100.0f};
    static constexpr auto REF_TEMPERATURE = Temperature(25.0f);
    static constexpr bool PullDown{true};
};

struct CircuitConfig
{
    static constexpr auto MIN_TEMPERATURE = Temperature(-10.0f);
    static constexpr auto MAX_TEMPERATURE = Temperature(100.0f);
    static constexpr uint32_t COUNT{24u};
    static constexpr auto SUPPLY_VOLTAGE = Volt(3.3f);
    static constexpr auto PRE_RESISTANCE = Ohm(10000.0f);
};

constexpr uint8_t ADC_RESOLUTION = 12u;
constexpr auto Conversion = [](auto value) { return static_cast<float>(value) * CircuitConfig::SUPPLY_VOLTAGE() * 1000.0f / Math::pow(2, ADC_RESOLUTION); };

int main()
{
    constexpr auto ntcPoints = NTC::samplingPointCalculator<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
    fmt::print("Count: {0}\n", CircuitConfig::COUNT);
    for (const auto& t : ntcPoints)
    {
        fmt::print("{0} - {1}mV\n", t, Conversion(t));
    }

    // dump the circuit as ASCII
    NTC::Draw::dump<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
}
