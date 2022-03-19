#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <fmt/format.h>

namespace Math {
constexpr float pow(float x, int y) { return y == 0 ? 1.0f : x * pow(x, y - 1); }

constexpr int factorial(int x) { return x == 0 ? 1 : x * factorial(x - 1); }

constexpr float exp(float x)
{
    float temp = 1.0f + x;
    for (int i = 2; i <= 9; i++)
    {
        temp += pow(x, i) / static_cast<float>(factorial(i));
    }
    return temp;
}
}  // namespace Math

using Temperature = float;
using Ohm = float;
using Volt = float;

namespace NTC {
struct OhmTemperature
{
    constexpr OhmTemperature() = default;
    constexpr OhmTemperature(Ohm resistance, Temperature temp) : resistance(resistance), temp(temp){};
    Ohm resistance = {0.0f};
    Temperature temp = {0.0f};
};

struct VoltTemperature
{
    constexpr VoltTemperature() = default;
    constexpr VoltTemperature(Volt voltage, Temperature temp) : voltage(voltage), temp(temp){};
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
        OhmTemperature temp(resistance, temperatureStep);
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
        constexpr Volt voltage = CircuitConfig::SUPPLY_VOLTAGE * (resistance.resistance + CircuitConfig::PRE_RESISTANCE);
        if constexpr (NTCConfig::PullDown)
        {
            voltage *= CircuitConfig::PRE_RESISTANCE;
        }
        else
        {
            voltage *= resistance.resistance;
        }
        VoltTemperature temp(voltage, resistance.temp);
        voltages[index] = temp;
        index++;
    }
    return voltages;
}

template<typename CircuitConfig, typename NTCConfig, uint8_t AdcResolution> constexpr auto samplingPointCalculator()
{
    constexpr auto powAdc = Math::pow(2, AdcResolution);
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

namespace Draw {
constexpr void resistance(Ohm resistor, std::string_view custom = "")
{
    for (int i = 0; i < 5; i++)
    {
        if (i == 2)
        {
            fmt::print("{0:>{1}}{0:>{2}}{3:>{2}} R1 {5} = {4} Ohm\n", "|", 18, 4, "", resistor, custom);
        }
        else
        {
            fmt::print("{0:>{1}}{0:>{2}}\n", "|", 18, 4);
        }
    }
}

template<typename CircuitConfig, typename NTCConfig, uint8_t AdcResolution> constexpr void dump()
{
    fmt::print("{4:.1f}V{0:-^{1}}\n{2:>{3}}\n{2:>{3}}\n{2:>{3}}\n", "", 15, "|", 20, CircuitConfig::SUPPLY_VOLTAGE);
    if constexpr (NTCConfig::PullDown)
    {
        resistance(CircuitConfig::PRE_RESISTANCE);
    }
    else
    {
        resistance(NTCConfig::RESISTANCE, "(NTC)");
    }

    fmt::print("{1:>{0}}\n{1:>{0}} {2:-^{0}}-> {3} Bit ADC\n{1:>{0}}\n", 20, "|", "", AdcResolution);

    if constexpr (NTCConfig::PullDown)
    {
        resistance(NTCConfig::RESISTANCE, "(NTC)");
    }
    else
    {
        resistance(CircuitConfig::PRE_RESISTANCE);
    }

    fmt::print("{2:>{1}}\n{2:>{1}}\n{2:>{1}}\n{0:-^{3}}\n", "", 20, "|", 19);
}
}  // namespace Draw
}  // namespace NTC
