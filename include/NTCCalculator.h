#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <fmt/format.h>

template<typename T>
class NamedType
{
  public:
    using Type = T;
    NamedType() = default;
    constexpr explicit NamedType(const T &value) : m_value(value)
    {}
    constexpr explicit NamedType(T &&value) : m_value(std::move(value))
    {}

    constexpr NamedType &operator=(T value)
    {
        m_value = value;
        return *this;
    }
    constexpr T operator()() const
    {
        return m_value;
    }

  private:
    T m_value{};
};

namespace Math {
constexpr auto EXP_COUNT = 9;

[[nodiscard]] constexpr float pow(float x, int y)
{
    return y == 0 ? 1.0f : x * pow(x, y - 1);
}

[[nodiscard]] constexpr int factorial(int x)
{
    return x == 0 ? 1 : x * factorial(x - 1);
}

[[nodiscard]] constexpr float exp(float x)
{
    float temp = 1.0f + x;
    for (int i = 2; i <= EXP_COUNT; i++) {
        temp += pow(x, i) / static_cast<float>(factorial(i));
    }
    return temp;
}

}// namespace Math

struct Temperature : NamedType<float>
{
    using NamedType::NamedType;
};
struct Ohm : NamedType<float>
{
    using NamedType::NamedType;
};
struct Volt : NamedType<float>
{
    using NamedType::NamedType;
};

namespace unit_literals {
constexpr auto operator""_Temp(long double d)
{
    return Temperature{ static_cast<Temperature::Type>(d) };
}
constexpr auto operator""_Ohm(long double d)
{
    return Ohm{ static_cast<Ohm::Type>(d) };
}
constexpr auto operator""_Volt(long double d)
{
    return Volt{ static_cast<Volt::Type>(d) };
}
}// namespace unit_literals

namespace NTC {
struct OhmTemperature
{
    OhmTemperature() = default;
    constexpr OhmTemperature(const Ohm &resist, const Temperature &temperature) : resistance(resist()), temp(temperature()) {};
    constexpr OhmTemperature(float resist, float temperature) : resistance(resist), temp(temperature) {};
    Ohm resistance;
    Temperature temp;
};

struct VoltTemperature
{
    VoltTemperature() = default;
    constexpr VoltTemperature(const Volt &volt, const Temperature &temperature) : voltage(volt()), temp(temperature()) {};
    constexpr VoltTemperature(float volt, float temperature) : voltage(volt), temp(temperature) {};
    Volt voltage;
    Temperature temp;
};

inline constexpr Temperature OFFSET{ 273.15f };

template<typename CircuitConfig, typename NTCConfig, bool IntegrateOffset = true>
[[nodiscard]] constexpr auto resistance()
{
    std::array<OhmTemperature, CircuitConfig::COUNT> resistances;
    constexpr auto REF_TEMP_OFFSET = OFFSET() + NTCConfig::REF_TEMPERATURE();
    constexpr auto TEMP_DIFF = (CircuitConfig::MAX_TEMPERATURE() - CircuitConfig::MIN_TEMPERATURE()) / CircuitConfig::COUNT;
    for (uint32_t i = 0; i < CircuitConfig::COUNT; ++i) {
        const float TEMPERATURE_STEP = (CircuitConfig::MIN_TEMPERATURE() + static_cast<float>(i) * TEMP_DIFF);
        const float RESISTANCE = NTCConfig::RESISTANCE() * Math::exp(NTCConfig::B_CONSTANT * (1.0f / (OFFSET() + TEMPERATURE_STEP) - 1.0f / REF_TEMP_OFFSET));
        OhmTemperature temp(RESISTANCE, TEMPERATURE_STEP);
        if constexpr (IntegrateOffset) {
            temp.temp = static_cast<Temperature>(OFFSET() + TEMPERATURE_STEP);
        }
        resistances[i] = temp;
    }
    return resistances;
}

template<typename CircuitConfig, typename NTCConfig>
[[nodiscard]] constexpr auto voltage()
{
    std::array<VoltTemperature, CircuitConfig::COUNT> voltages;
    [[maybe_unused]] size_t index = 0u;
    for ([[maybe_unused]] const auto &resist : resistance<CircuitConfig, NTCConfig>()) {
        [[maybe_unused]] auto voltage = 0.0f;
        if constexpr (NTCConfig::PULL_DOWN) {
            voltage = CircuitConfig::SUPPLY_VOLTAGE() * (CircuitConfig::PRE_RESISTANCE() / (resist.resistance() + CircuitConfig::PRE_RESISTANCE()));
        } else {
            voltage = CircuitConfig::SUPPLY_VOLTAGE() * (resist.resistance() / (resist.resistance() + CircuitConfig::PRE_RESISTANCE()));
        }
        voltages[index] = VoltTemperature(voltage, resist.temp());
        index++;
    }
    return voltages;
}

template<typename CircuitConfig, typename NTCConfig, uint8_t AdcResolution>
[[nodiscard]] constexpr auto samplingPointCalculator()
{
    [[maybe_unused]] constexpr auto POW_ADC = Math::pow(2, AdcResolution);
    std::array<uint16_t, CircuitConfig::COUNT> samplingPoints = { 0 };
    [[maybe_unused]] size_t index = 0u;
    for ([[maybe_unused]] const auto &volt : voltage<CircuitConfig, NTCConfig>()) {
        samplingPoints[index] = static_cast<uint16_t>((POW_ADC * volt.voltage()) / CircuitConfig::SUPPLY_VOLTAGE());
        ++index;
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
        constexpr std::string_view RESISTOR_STRING = "{0:>{1}}{0:>{2}}{3:>{2}} R1 {5} = {4} Ohm\n";
        constexpr std::string_view INDENTATION_STRING = "{0:>{1}}{0:>{2}}\n";
        fmt::print("{0:>{1}}\n", "---", PRE_INDENTATION + 3);
        for (int i = 0; i < RESISTOR_HEIGHT; i++) {
            if (i == RESISTOR_DEFINITION) {
                fmt::print(RESISTOR_STRING, "|", PRE_INDENTATION, 4, "", resistor(), custom);
                continue;
            }
            fmt::print(INDENTATION_STRING, "|", PRE_INDENTATION, 4);
        }
        fmt::print("{0:>{1}}\n", "---", PRE_INDENTATION + 3);
    }

    template<typename CircuitConfig, typename NTCConfig, uint8_t AdcResolution>
    constexpr void dump()
    {
        fmt::print("{4:.1f}V{0:-^{1}}\n{2:>{3}}\n{2:>{3}}\n{2:>{3}}\n", "", 15, "|", INDENTATION_OFFSET, CircuitConfig::SUPPLY_VOLTAGE());
        if constexpr (NTCConfig::PULL_DOWN) {
            resistance(CircuitConfig::PRE_RESISTANCE);
        } else {
            resistance(NTCConfig::RESISTANCE, NTC);
        }

        fmt::print("{1:>{0}}\n{1:>{0}} {2:-^{0}}-> {3} Bit ADC\n{1:>{0}}\n", INDENTATION_OFFSET, "|", "", AdcResolution);

        if constexpr (NTCConfig::PULL_DOWN) {
            resistance(NTCConfig::RESISTANCE, NTC);
        } else {
            resistance(CircuitConfig::PRE_RESISTANCE);
        }

        fmt::print("{2:>{1}}\n{2:>{1}}\n{2:>{1}}\n{0:-^{3}}\n", "", INDENTATION_OFFSET, "|", 19);
    }
}// namespace Draw
}// namespace NTC
