# NTC calculation during compile time

![C++ workflow](https://github.com/StephanKa/ntc_sample_calculator/actions/workflows/build_cmake.yml/badge.svg)
![CodeQL](https://github.com/StephanKa/ntc_sample_calculator/workflows/CodeQL/badge.svg)

## General

This repository shows a simple sampling point calculator for NTC's which will be executed during compile time.

## Examples

### Configuration

#### Type definition

```c++
using Temperature = float;
using Ohm = float;
using Volt = float;
```

#### NTC configuration

```c++
struct NTCConfig
{
    static constexpr Temperature REF_TEMPERATURE = {25.0f}; // reference temperature (see datasheet)
    static constexpr Ohm RESISTANCE = {10000.0f}; // describes the NTC at defined REF_TEMPERATURE
    static constexpr float B_CONSTANT = {4100.0f}; // see datasheet for beta constant
    static constexpr bool PullDown = {true}; // defines if the NTC is pull-up <false> or pull-down <true>
};
```

#### Circuit configuration

```c++
struct CircuitConfig
{
    static constexpr Temperature MIN_TEMPERATURE = -10.0f; // minimum temperature for sampling points
    static constexpr Temperature MAX_TEMPERATURE = 100.0f; // minimum temperature for sampling points
    static constexpr uint32_t COUNT = 24; // sampling count, temperature steps will be automatically calculated
    static constexpr Volt SUPPLY_VOLTAGE = {3.3f}; // defines the voltage given
    static constexpr Ohm PRE_RESISTANCE = {10000.0f}; // defines the preseries resistor
};

```

#### Return types

```c++
struct OhmTemperature
{
    Ohm ohm;
    Temperature temp;
};

struct VoltTemperature
{
    Volt voltage;
    Temperature temp;
};
```

#### Methods

```c++
constexpr bool IntegrateTempOffset = true; // defines if temperature shall be compensated to K or leave a °C
// calculate only the resistances for given temperature steps
constexpr NTC::OhmTemperature resistances = NTC::resistance<CircuitConfig, NTCConfig, IntegrateTempOffset>();
// calculate only the volatges for given temperature steps
constexpr NTC::VoltTemperature voltages = NTC::voltage<CircuitConfig, NTCConfig>();
// calculate only the resistances for given temperature steps
constexpr uint8_t ADC_RESOLUTION = 12u;
constexpr auto ntcSamplingPoints = NTC::samplingPointCalculator<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
```

### Example

For this example check out the **source/main.cpp**

```c++
struct NTCConfig
{
    static constexpr Ohm RESISTANCE = {10000.0f};
    static constexpr float B_CONSTANT = {4100.0f};
    static constexpr Temperature REF_TEMPERATURE = {25.0f};
    static constexpr bool PullDown = {true};
};

struct CircuitConfig
{
    static constexpr Temperature MIN_TEMPERATURE = -10.0f;
    static constexpr Temperature MAX_TEMPERATURE = 100.0f;
    static constexpr uint32_t COUNT = 24;
    static constexpr Volt SUPPLY_VOLTAGE = {3.3f};
    static constexpr Ohm PRE_RESISTANCE = {10000.0f};
};

constexpr uint8_t ADC_RESOLUTION = 12u;

int main()
{
    constexpr auto ntcPoints = NTC::samplingPointCalculator<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
    fmt::print("Count: {0}\n", CircuitConfig::COUNT);
    for (const auto& t : ntcPoints)
    {
        fmt::print("{0} - {1}mV\n", t, t * CircuitConfig::SUPPLY_VOLTAGE * 1000.0f / Math::pow(2, ADC_RESOLUTION));
    }
}

```

## Output

```c++
constexpr OhmTemperature resistances = NTC::resistance<CircuitConfig, NTCConfig, IntegrateTempOffset>();
```

[Compiler Explorer: GCC 11.2 & Clang 12.0.1](https://godbolt.org/z/PGxqnM196)

```c++
constexpr VoltTemperature voltages = NTC::voltage<CircuitConfig, NTCConfig>();
```

[Compiler Explorer: GCC 11.2 & Clang 12.0.1](https://godbolt.org/z/479coovdd)

```c++
constexpr uint8_t ADC_RESOLUTION = 12u;
constexpr auto ntcSamplingPoints = NTC::samplingPointCalculator<CircuitConfig, NTCConfig, ADC_RESOLUTION>();
```

[Compiler Explorer: GCC 11.2 & Clang 12.0.1](https://godbolt.org/z/aT6GY7K1Y)

## To-Do

- [ ] add small ascii circuit output 
