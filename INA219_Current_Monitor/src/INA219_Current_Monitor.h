/*
 Name:		INA219_Current_Monitor.h
 Created:	31/12/2019 8:10:10 AM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#ifndef _INA219_Current_Monitor_h
#define _INA219_Current_Monitor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#define INA219_DEFAULT_ADDRESS 0x40

#ifndef ENUM_CONVERSION
#define ENUM_CONVERSION 1

template<typename Enumerator>
uint8_t getEnumValue(const Enumerator& enumType)
{
	return static_cast<uint8_t>(enumType);
}

#endif // ENUM_CONVERSION

class INA219 final
{
public:
	/// <summary>
	/// Registers available in INA219.
	/// </summary>
	enum class registers : uint8_t { configuration, shuntVoltage, busVoltage, power, current, calibration };

	/// <summary>
	/// Bus voltage range. Can be either 16V or 32V.
	/// </summary>
	enum class busVoltageRange : uint8_t { V16, V32 };

	/// <summary>
	/// Gain setting of Programmable Gain Amplifier. 1: +-40mV, 2: +-80mV, 4: +-160mV, 8: +-320mV.
	/// </summary>
	enum class pgaGain : uint8_t { g1, g2, g4, g8 };

	/// <summary>
	/// ADC resolution settings for one sample mode. From 9 to 12 bit.
	/// </summary>
	enum class adcResolution : uint8_t { b9, b10, b11, b12 };

	/// <summary>
	/// Number of averaged samples for averaging mode. From 2 to 128 samples.
	/// </summary>
	enum class adcAveragedSamples : uint8_t { s2 = 8, s4, s8, s16, s32, s64, s128 };

	/// <summary>
	/// Possible operating modes.
	/// </summary>
	enum class operatingMode : uint8_t {
		powerDown,
		shuntVoltage_triggered,
		busVoltage_triggered,
		shuntAndBusVoltage_triggered,
		adcOff,
		shuntVoltage_continuous,
		busVoltage_continuous,
		shuntAndBusVoltage_continuous
	};

public:
	/// <summary>
	/// Constructor of INA219 class objects.
	/// </summary>
	/// <param name="i2cAddress">I2C address of INA219</param>
	/// <param name="maxExpectedCurrentInMilli">Max expected current value to measure in milliAmperes</param>
	/// <param name="rShuntValueInMilli">Shunt resistor value in milliOhms</param>
	/// <param name="busVoltage">Bus voltage setting</param>
	/// <param name="pgaGain">PGA gain setting</param>
	/// <param name="operatingMode">Operating mode setting</param>
	INA219(const uint8_t& i2cAddress,
		const uint16_t& maxExpectedCurrentInMilli,
		const uint16_t& rShuntValueInMilli,
		busVoltageRange busVoltage = INA219::busVoltageRange::V16,
		pgaGain pgaGain = INA219::pgaGain::g4,
		operatingMode operatingMode = INA219::operatingMode::shuntAndBusVoltage_continuous) : m_i2cAddress(i2cAddress), m_rShuntValue(rShuntValueInMilli)
	{
		setBusVoltageRange(busVoltage);
		setPgaGain(pgaGain);
		setOperatingMode(operatingMode);
		setRegisterValue(registers::calibration, calculateCalibrationValue(maxExpectedCurrentInMilli));
	}

	INA219(const INA219&) = delete;
	INA219& operator=(const INA219&) = delete;

public:
	/// <summary>
	/// Sets value of a selected register.
	/// </summary>
	/// <param name="reg">Selected register</param>
	/// <param name="registerValue">Value to write to register</param>
	void setRegisterValue(const registers& reg, const uint16_t& registerValue);

	/// <summary>
	/// Gets value of a selected register.
	/// </summary>
	/// <param name="reg">Selected register</param>
	/// <returns>Value from register</returns>
	uint16_t getRegisterValue(const registers& reg);

	/// <summary>
	/// Calculates calibration value.
	/// </summary>
	/// <param name="maxExpectedCurrentInMilli">Max expected current value to measure in milliAmperes</param>
	/// <returns>Calibration value</returns>
	uint16_t calculateCalibrationValue(const uint16_t& maxExpectedCurrentInMilli);

#pragma region Configuration Register
	/// <summary>
	/// Sends a reset command. It will cause INA219 to reset and all registers will be changed to default values.
	/// </summary>
	void resetDevice(void);

	/// <summary>
	/// Changes bus voltage range.
	/// </summary>
	/// <param name="range">Bus voltage range</param>
	void setBusVoltageRange(const busVoltageRange& range);

	/// <summary>
	/// Sets PGA gain.
	/// </summary>
	/// <param name="gain">Gain of the PGA</param>
	void setPgaGain(const pgaGain& gain);

	/// <summary>
	/// Sets ADC resolution. After using this option, device will be configured to take samples at given resolution without averaging.
	/// </summary>
	/// <param name="resolution">Chosen resolution of ADC</param>
	/// <param name="busSetting">If true then Bus ADC setting will be changed, otherwise Shunt ADC setting will be changed</param>
	void setAdcResolution(const adcResolution& resolution, const bool& busSetting = false);

	/// <summary>
	/// Sets number of ADC samples to average. After using this option, device will be configured to average given number of samples at 12 bit resolution.
	/// </summary>
	/// <param name="samplesNumber">Number of samples to average</param>
	/// <param name="busSetting">If true then Bus ADC setting will be changed, otherwise Shunt ADC setting will be changed</param>
	void setNumberOfAveragedSamples(const adcAveragedSamples& samplesNumber, const bool& busSetting = false);

	/// <summary>
	/// Changes device operating mode.
	/// </summary>
	/// <param name="mode">Selected operating mode of INA219</param>
	void setOperatingMode(const operatingMode& mode);
#pragma endregion
#pragma region Shunt Voltage Register
	/// <summary>
	/// Gets shunt voltage in milliVolts.
	/// </summary>
	/// <returns>Value of measured shunt voltage in milliVolts</returns>
	double getShuntVoltageValue(void);
#pragma endregion
#pragma region Bus Voltage Register
	/// <summary>
	/// Gets bus voltage in Volts.
	/// </summary>
	/// <returns>Value of measured bus voltage in Volts</returns>
	double getBusVoltageValue(void);

	/// <summary>
	/// Gets state of conversion ready flag.
	/// </summary>
	/// <returns>True if conversion ready flag has been set, false otherwise</returns>
	bool getConversionReadyState(void);

	/// <summary>
	/// Gets state of math overflow flag.
	/// </summary>
	/// <returns>True if math overflow flag has been set, false otherwise</returns>
	bool getMathOverflowFlag(void);
#pragma endregion
#pragma region Power Register
	/// <summary>
	/// Gets value of power in Watts.
	/// </summary>
	/// <returns>Value of measured power in Watts</returns>
	double getPowerValue(void);
#pragma endregion
#pragma region Current Register
	/// <summary>
	/// Gets current value in Amperes.
	/// </summary>
	/// <returns>Value of measured current in Amperes</returns>
	double getCurrentValue(void);
#pragma endregion

private:
	/// <summary>
	/// I2C address of INA219.
	/// </summary>
	const uint8_t m_i2cAddress;

	/// <summary>
	/// Shunt resistor value in milliOhms.
	/// </summary>
	const uint16_t m_rShuntValue;

	/// <summary>
	/// Value of current LSB.
	/// </summary>
	double m_currentLsb;
};

#endif