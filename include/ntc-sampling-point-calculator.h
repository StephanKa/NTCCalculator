#pragma once
#include <array>
#include <cmath>

namespace Math
{
constexpr float pow(float x, int y)
{
    return y == 0 ? 1.0f : x * pow(x, y - 1);
}

constexpr int factorial(int x)
{
    return x == 0 ? 1 : x * factorial(x - 1);
}

constexpr float exp(float x)
{
    return 1.0f + x + pow(x, 2) / factorial(2) + pow(x, 3) / factorial(3) + pow(x, 4) / factorial(4) + pow(x, 5) / factorial(5) + pow(x, 6) / factorial(6) +
        pow(x, 7) / factorial(7) + pow(x, 8) / factorial(8) + pow(x, 9) / factorial(9);
}
}

constexpr float MIN_TEMPERATURE = -10.0f;
constexpr float MAX_TEMPERATURE = 100.0f;
constexpr uint32_t FACTOR_SAMPLING_POINTS = 20;
constexpr uint32_t COUNT = static_cast<uint32_t>(MAX_TEMPERATURE - MIN_TEMPERATURE + 1) * FACTOR_SAMPLING_POINTS;
constexpr float SUPPLY_VOLTAGE = {3.3f};
constexpr float RESISTANCE = {10000.0f};
constexpr float B_CONSTANT = {4100.0f};
constexpr float REF_TEMPERATURE = {25.0f};
constexpr uint8_t ADC_RESOLUTION = {12u};
constexpr float OFFSET = {273.15f};

template <auto N, bool PullDown>
constexpr std::array<uint16_t, N> ntcSamplingPointCalculator()
{
    std::array<uint16_t, N> samplingPoints {0};
    for (auto i = 0; i != N; ++i)
    {
        float temperatureStep = (MIN_TEMPERATURE + i * (MAX_TEMPERATURE - MIN_TEMPERATURE) / N);
        float resistance = RESISTANCE * Math::exp(B_CONSTANT * (1.0f / (OFFSET + temperatureStep) - 1.0f / (OFFSET + REF_TEMPERATURE)));
        float voltage = {0.0f};
        if constexpr (PullDown)
        {
            voltage = SUPPLY_VOLTAGE * (RESISTANCE / (resistance + RESISTANCE));
        }
        else
        {
            voltage = SUPPLY_VOLTAGE * (resistance / (resistance + RESISTANCE));
        }
        samplingPoints[i] = static_cast<uint16_t>((Math::pow(2, ADC_RESOLUTION) * voltage) / SUPPLY_VOLTAGE);
    }
    return samplingPoints;
}