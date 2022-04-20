#include <NTCCalculator.h>
#include <catch2/catch.hpp>

struct NTCConfig
{
    static constexpr auto RESISTANCE = Ohm(10000.0f);
    static constexpr float B_CONSTANT{4100.0f};
    static constexpr auto REF_TEMPERATURE = Temperature(25.0f);
    static constexpr bool PullDown{false};
};

struct CircuitConfig
{
    static constexpr auto MIN_TEMPERATURE = Temperature(20.0f);
    static constexpr auto MAX_TEMPERATURE = Temperature(100.0f);
    static constexpr uint32_t COUNT{24u};
    static constexpr auto SUPPLY_VOLTAGE = Volt(3.3f);
    static constexpr auto PRE_RESISTANCE = Ohm(10000.0f);
};

TEST_CASE("Resistance", "TestCalculation")
{
    constexpr auto ntcPoints = NTC::resistance<CircuitConfig, NTCConfig>();
    std::array<uint32_t, CircuitConfig::COUNT> refValues = {12643u, 10803u, 9264u, 7970u, 6880u, 5958u, 5175u, 4508u, 3939u, 3451u, 3031u, 2670u,
                                                            2358u,  2087u,  1851u, 1646u, 1467u, 1309u, 1169u, 1046u, 935u,  834u,  742u,  656u};
    size_t index = 0u;
    for (const auto& ref : refValues)
    {
        REQUIRE(static_cast<uint32_t>(ntcPoints.at(index).resistance()) == ref);
        index++;
    }
}
