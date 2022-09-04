#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <fmt/format.h>
#include <functional>
#include <vector>

template<typename T> class NamedType
{
public:
    NamedType() = default;
    constexpr explicit NamedType(const T& value) : m_value(value) {}
    constexpr explicit NamedType(T&& value) : m_value(std::move(value)) {}

    constexpr NamedType& operator=(T value)
    {
        m_value = value;
        return *this;
    }
    constexpr T operator()() const { return m_value; }

private:
    T m_value{};
};

namespace Math {
constexpr auto EXP_COUNT = 9;

constexpr float pow(float x, int y) { return y == 0 ? 1.0f : x * pow(x, y - 1); }

constexpr int factorial(int x) { return x == 0 ? 1 : x * factorial(x - 1); }

constexpr float exp(float x)
{
    float temp = 1.0f + x;
    for (int i = 2; i <= EXP_COUNT; i++)
    {
        temp += pow(x, i) / static_cast<float>(factorial(i));
    }
    return temp;
}

}  // namespace Math

using Temperature = NamedType<float>;
using Ohm = NamedType<float>;
using Volt = NamedType<float>;

namespace NTC {
struct OhmTemperature
{
    OhmTemperature() = default;
    constexpr OhmTemperature(const Ohm& resist, const Temperature& temperature) : resistance(resist()), temp(temperature()){};
    constexpr OhmTemperature(float resist, float temperature) : resistance(resist), temp(temperature){};
    Ohm resistance;
    Temperature temp;
};

struct VoltTemperature
{
    VoltTemperature() = default;
    constexpr VoltTemperature(const Volt& volt, const Temperature& temperature) : voltage(volt()), temp(temperature()){};
    constexpr VoltTemperature(float volt, float temperature) : voltage(volt), temp(temperature){};
    Volt voltage;
    Temperature temp;
};

static constexpr Temperature OFFSET{273.15f};

template<typename CircuitConfig, typename NTCConfig, bool IntegrateOffset = true> constexpr auto resistance()
{
    std::array<OhmTemperature, CircuitConfig::COUNT> resistances;
    constexpr auto REF_TEMP_OFFSET = OFFSET() + NTCConfig::REF_TEMPERATURE();
    constexpr auto TEMP_DIFF = (CircuitConfig::MAX_TEMPERATURE() - CircuitConfig::MIN_TEMPERATURE()) / CircuitConfig::COUNT;
    for (uint32_t i = 0; i < CircuitConfig::COUNT; ++i)
    {
        const float TEMPERATURE_STEP = (CircuitConfig::MIN_TEMPERATURE() + static_cast<float>(i) * TEMP_DIFF);
        const float RESISTANCE = NTCConfig::RESISTANCE() * Math::exp(NTCConfig::B_CONSTANT * (1.0f / (OFFSET() + TEMPERATURE_STEP) - 1.0f / REF_TEMP_OFFSET));
        OhmTemperature temp(RESISTANCE, TEMPERATURE_STEP);
        if constexpr (IntegrateOffset)
        {
            temp.temp = OFFSET() + TEMPERATURE_STEP;
        }
        resistances[i] = temp;
    }
    return resistances;
}

template<typename CircuitConfig, typename NTCConfig> constexpr auto voltage()
{
    std::array<VoltTemperature, CircuitConfig::COUNT> voltages;
    [[maybe_unused]] size_t index = 0u;
    for ([[maybe_unused]] const auto& resist : resistance<CircuitConfig, NTCConfig>())
    {
        [[maybe_unused]] auto voltage = 0.0f;
        if constexpr (NTCConfig::PullDown)
        {
            voltage = CircuitConfig::SUPPLY_VOLTAGE() * (CircuitConfig::PRE_RESISTANCE() / (resist.resistance() + CircuitConfig::PRE_RESISTANCE()));
        }
        else
        {
            voltage = CircuitConfig::SUPPLY_VOLTAGE() * (resist.resistance() / (resist.resistance() + CircuitConfig::PRE_RESISTANCE()));
        }
        voltages[index] = VoltTemperature(voltage, resist.temp());
        index++;
    }
    return voltages;
}

template<typename CircuitConfig, typename NTCConfig, uint8_t AdcResolution> constexpr auto samplingPointCalculator()
{
    [[maybe_unused]] constexpr auto POW_ADC = Math::pow(2, AdcResolution);
    std::array<uint16_t, CircuitConfig::COUNT> samplingPoints = {0};
    [[maybe_unused]] size_t index = 0u;
    for ([[maybe_unused]] const auto& volt : voltage<CircuitConfig, NTCConfig>())
    {
        samplingPoints[index] = static_cast<uint16_t>((POW_ADC * volt.voltage()) / CircuitConfig::SUPPLY_VOLTAGE());
        index++;
    }
    return samplingPoints;
}

namespace Draw {
constexpr std::string_view NTC = "(NTC)";
constexpr auto PRE_INDENTATION = 18;
constexpr auto RESISTOR_HEIGHT = 5;
constexpr auto RESISTOR_DEFINITION = 2;
constexpr auto INDENTATION_OFFSET = 20;

constexpr void resistance(Ohm resistor, std::string_view custom = "")
{
    for (int i = 0; i < RESISTOR_HEIGHT; i++)
    {
        if (i == RESISTOR_DEFINITION)
        {
            fmt::print("{0:>{1}}{0:>{2}}{3:>{2}} R1 {5} = {4} Ohm\n", "|", PRE_INDENTATION, 4, "", resistor(), custom);
        }
        else
        {
            fmt::print("{0:>{1}}{0:>{2}}\n", "|", PRE_INDENTATION, 4);
        }
    }
}

template<typename CircuitConfig, typename NTCConfig, uint8_t AdcResolution> constexpr void dump()
{
    fmt::print("{4:.1f}V{0:-^{1}}\n{2:>{3}}\n{2:>{3}}\n{2:>{3}}\n", "", 15, "|", INDENTATION_OFFSET, CircuitConfig::SUPPLY_VOLTAGE());
    if constexpr (NTCConfig::PullDown)
    {
        resistance(CircuitConfig::PRE_RESISTANCE);
    }
    else
    {
        resistance(NTCConfig::RESISTANCE, NTC);
    }

    fmt::print("{1:>{0}}\n{1:>{0}} {2:-^{0}}-> {3} Bit ADC\n{1:>{0}}\n", INDENTATION_OFFSET, "|", "", AdcResolution);

    if constexpr (NTCConfig::PullDown)
    {
        resistance(NTCConfig::RESISTANCE, NTC);
    }
    else
    {
        resistance(CircuitConfig::PRE_RESISTANCE);
    }

    fmt::print("{2:>{1}}\n{2:>{1}}\n{2:>{1}}\n{0:-^{3}}\n", "", INDENTATION_OFFSET, "|", 19);
}
}  // namespace Draw
}  // namespace NTC
