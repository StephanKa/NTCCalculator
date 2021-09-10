#pragma once
#include <array>
#include <cmath>
#include <cstdint>

namespace Math {
constexpr float pow(float x, int y) { return y == 0 ? 1.0f : x * pow(x, y - 1); }

constexpr int factorial(int x) { return x == 0 ? 1 : x * factorial(x - 1); }

constexpr float exp(float x)
{
    return 1.0f + x + pow(x, 2) / factorial(2) + pow(x, 3) / factorial(3) + pow(x, 4) / factorial(4) + pow(x, 5) / factorial(5) + pow(x, 6) / factorial(6)
           + pow(x, 7) / factorial(7) + pow(x, 8) / factorial(8) + pow(x, 9) / factorial(9);
}
}  // namespace Math

using Temperature = float;
using Ohm = float;
using Volt = float;

namespace NTC {
struct OhmTemperature
{
    Ohm resistance = {0.0f};
    Temperature temp = {0.0f};
};

struct VoltTemperature
{
    Volt voltage = {0.0f};
    Temperature temp = {0.0f};
};

static constexpr Temperature OFFSET = {273.15f};

template<typename CircuitConfig, typename NTCConfig, bool IntegrateOffset = true> constexpr auto resistance()
{
    std::array<OhmTemperature, CircuitConfig::COUNT> resistances;
    constexpr auto refTempOffset = OFFSET + NTCConfig::REF_TEMPERATURE;
    constexpr auto tempDiff = (CircuitConfig::MAX_TEMPERATURE - CircuitConfig::MIN_TEMPERATURE) / CircuitConfig::COUNT;
    for (uint32_t i = 0; i < CircuitConfig::COUNT; ++i)
    {
        const float temperatureStep = (CircuitConfig::MIN_TEMPERATURE + static_cast<float>(i) * tempDiff);
        const float resistance = NTCConfig::RESISTANCE * Math::exp(NTCConfig::B_CONSTANT * (1.0f / (OFFSET + temperatureStep) - 1.0f / refTempOffset));
        OhmTemperature temp{};
        temp.resistance = resistance;
        temp.temp = temperatureStep;
        if constexpr (IntegrateOffset)
        {
            temp.temp = OFFSET + temperatureStep;
        }
        resistances[i] = temp;
    }
    return resistances;
}

template<typename CircuitConfig, typename NTCConfig> constexpr auto voltage()
{
    std::array<VoltTemperature, CircuitConfig::COUNT> voltages;
    constexpr auto resistances = resistance<CircuitConfig, NTCConfig>();
    size_t index = 0u;
    for (const auto& resistance : resistances)
    {
        Volt voltage = {0.0f};
        if constexpr (NTCConfig::PullDown)
        {
            voltage = CircuitConfig::SUPPLY_VOLTAGE * (CircuitConfig::PRE_RESISTANCE / (resistance.resistance + CircuitConfig::PRE_RESISTANCE));
        }
        else
        {
            voltage = CircuitConfig::SUPPLY_VOLTAGE * (resistance.resistance / (resistance.resistance + CircuitConfig::PRE_RESISTANCE));
        }
        VoltTemperature temp{};
        temp.voltage = voltage;
        temp.temp = resistance.temp;
        voltages[index] = temp;
        index++;
    }
    return voltages;
}

template<typename CircuitConfig, typename NTCConfig, uint8_t ADC_RESOLUTION> constexpr auto samplingPointCalculator()
{
    constexpr auto powAdc = Math::pow(2, ADC_RESOLUTION);
    constexpr auto voltages = voltage<CircuitConfig, NTCConfig>();
    std::array<uint16_t, CircuitConfig::COUNT> samplingPoints = {0};
    size_t index = 0u;
    for (const auto& volt : voltages)
    {
        samplingPoints[index] = static_cast<uint16_t>((powAdc * volt.voltage) / CircuitConfig::SUPPLY_VOLTAGE);
        index++;
    }
    return samplingPoints;
}
}  // namespace NTC