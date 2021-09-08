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

template<typename CircuitConfig, typename NTCConfig, uint8_t ADC_RESOLUTION> constexpr auto ntcSamplingPointCalculator()
{
    std::array<uint16_t, CircuitConfig::COUNT> samplingPoints = {0};
    constexpr auto powAdc = Math::pow(2, ADC_RESOLUTION);
    constexpr auto refTempOffset = NTCConfig::OFFSET + NTCConfig::REF_TEMPERATURE;
    constexpr auto tempDiff = (CircuitConfig::MAX_TEMPERATURE - CircuitConfig::MIN_TEMPERATURE) / CircuitConfig::COUNT;
    for (uint32_t i = 0; i < CircuitConfig::COUNT; ++i)
    {
        const float temperatureStep = (CircuitConfig::MIN_TEMPERATURE + static_cast<float>(i) * tempDiff);
        const float resistance = NTCConfig::RESISTANCE * Math::exp(NTCConfig::B_CONSTANT * (1.0f / (NTCConfig::OFFSET + temperatureStep) - 1.0f / refTempOffset));
        float voltage = {0.0f};
        if constexpr (NTCConfig::PullDown)
        {
            voltage = CircuitConfig::SUPPLY_VOLTAGE * (CircuitConfig::PRE_RESISTANCE / (resistance + CircuitConfig::PRE_RESISTANCE));
        }
        else
        {
            voltage = CircuitConfig::SUPPLY_VOLTAGE * (resistance / (resistance + CircuitConfig::PRE_RESISTANCE));
        }
        samplingPoints[i] = static_cast<uint16_t>((powAdc * voltage) / CircuitConfig::SUPPLY_VOLTAGE);
    }
    return samplingPoints;
}
