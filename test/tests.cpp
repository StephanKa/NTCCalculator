#include <catch2/catch.hpp>
#include <ntc-sampling-point-calculator.h>

struct NTCConfig
{
    using Temperature = float;
    using Ohm = float;
    static constexpr Ohm RESISTANCE = {10000.0f};
    static constexpr float B_CONSTANT = {4100.0f};
    static constexpr Temperature REF_TEMPERATURE = {25.0f};
    static constexpr Temperature OFFSET = {273.15f};
    static constexpr bool PullDown = {false};
};

struct CircuitConfig
{
    using Temperature = float;
    using Ohm = float;
    static constexpr Temperature MIN_TEMPERATURE = 20.0f;
    static constexpr Temperature MAX_TEMPERATURE = 100.0f;
    static constexpr uint32_t COUNT = 24;
    static constexpr float SUPPLY_VOLTAGE = {3.3f};
    static constexpr Ohm PRE_RESISTANCE = {10000.0f};
};

TEST_CASE("Resistance", "TestCalculation")
{
    constexpr auto ntcPoints = NTC::resistance<CircuitConfig, NTCConfig>();
    std::array<uint32_t, CircuitConfig::COUNT> refValues = {12643u, 10803u, 9264u, 7970u, 6880u, 5958u, 5175u, 4508u, 3939u, 3451u, 3031u, 2670u,
                                                            2358u,  2087u,  1851u, 1646u, 1467u, 1309u, 1169u, 1046u, 935u,  834u,  742u,  656u};
    size_t index = 0u;
    for (const auto& ref : refValues)
    {
        REQUIRE(static_cast<uint32_t>(ntcPoints[index].resistance) == ref);
        index++;
    }
}