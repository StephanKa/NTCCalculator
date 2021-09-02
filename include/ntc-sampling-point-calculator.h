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

template<typename Config, uint8_t ADC_RESOLUTION> constexpr auto ntcSamplingPointCalculator()
{
    std::array<uint16_t, Config::COUNT> samplingPoints = {0};
    constexpr auto powAdc = Math::pow(2, ADC_RESOLUTION);
    constexpr auto refTempOffset = Config::OFFSET + Config::REF_TEMPERATURE;
    constexpr auto tempDiff = (Config::MAX_TEMPERATURE - Config::MIN_TEMPERATURE) / Config::COUNT;
    for (uint32_t i = 0; i < Config::COUNT; ++i)
    {
        const float temperatureStep = (Config::MIN_TEMPERATURE + static_cast<float>(i) * tempDiff);
        const float resistance = Config::RESISTANCE * Math::exp(Config::B_CONSTANT * (1.0f / (Config::OFFSET + temperatureStep) - 1.0f / refTempOffset));
        float voltage = {0.0f};
        if constexpr (Config::PullDown)
        {
            voltage = Config::SUPPLY_VOLTAGE * (Config::RESISTANCE / (resistance + Config::RESISTANCE));
        }
        else
        {
            voltage = Config::SUPPLY_VOLTAGE * (resistance / (resistance + Config::RESISTANCE));
        }
        samplingPoints[i] = static_cast<uint16_t>((powAdc * voltage) / Config::SUPPLY_VOLTAGE);
    }
    return samplingPoints;
}
