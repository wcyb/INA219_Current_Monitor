/*
 Name:		INA219_Current_Monitor.cpp
 Created:	31/12/2019 8:10:10 AM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#include "INA219_Current_Monitor.h"

void INA219::setRegisterValue(const registers& reg, const uint16_t& registerValue)
{
	Wire.beginTransmission(m_i2cAddress);
	Wire.write(getEnumValue(reg));
	Wire.write(static_cast<uint8_t>((registerValue & 0xFF00) >> 8));
	Wire.write(static_cast<uint8_t>(registerValue & 0x00FF));
	Wire.endTransmission();
}

uint16_t INA219::getRegisterValue(const registers& reg)
{
	uint16_t regValue = 0;

	Wire.beginTransmission(m_i2cAddress);
	Wire.write(getEnumValue(reg));
	Wire.endTransmission(false);
	Wire.requestFrom(m_i2cAddress, static_cast<uint8_t>(2));

	regValue = Wire.read();
	regValue <<= 8;
	regValue |= Wire.read();

	return regValue;
}

uint16_t INA219::calculateCalibrationValue(const uint16_t& maxExpectedCurrentInMilli)
{
	m_currentLsb = (maxExpectedCurrentInMilli / 1000.0) / 32768;
	return static_cast<uint16_t>(0.04096 / (m_currentLsb * (m_rShuntValue / 1000.0)));
}

void INA219::resetDevice(void)
{
	uint16_t currentSettings = getRegisterValue(registers::configuration);

	setRegisterValue(registers::configuration, currentSettings | 0x8000);
}

void INA219::setBusVoltageRange(const busVoltageRange& range)
{
	uint16_t currentSettings = getRegisterValue(registers::configuration) & 0xDFFF;//reset only changed setting
	uint16_t changedValue = getEnumValue(range);

	setRegisterValue(registers::configuration, currentSettings | (changedValue << 13));
}

void INA219::setPgaGain(const pgaGain& gain)
{
	uint16_t currentSettings = getRegisterValue(registers::configuration) & 0xE7FF;//reset only changed setting
	uint16_t changedValue = getEnumValue(gain);

	setRegisterValue(registers::configuration, currentSettings | (changedValue << 11));
}

void INA219::setAdcResolution(const adcResolution& resolution, const bool& busSetting)
{
	uint16_t currentSettings = getRegisterValue(registers::configuration);
	uint16_t changedValue = getEnumValue(resolution);

	if (busSetting)//shunt ADC
	{
		currentSettings &= 0xF87F;//reset only changed setting
		changedValue <<= 7;
	}
	else//bus ADC
	{
		currentSettings &= 0xFF87;
		changedValue <<= 3;
	}

	setRegisterValue(registers::configuration, currentSettings | changedValue);
}

void INA219::setNumberOfAveragedSamples(const adcAveragedSamples& samplesNumber, const bool& busSetting)
{
	uint16_t currentSettings = getRegisterValue(registers::configuration);
	uint16_t changedValue = getEnumValue(samplesNumber);

	if (busSetting)//shunt ADC
	{
		currentSettings &= 0xF87F;//reset only changed setting
		changedValue <<= 7;
	}
	else//bus ADC
	{
		currentSettings &= 0xFF87;
		changedValue <<= 3;
	}

	setRegisterValue(registers::configuration, currentSettings | changedValue);
}

void INA219::setOperatingMode(const operatingMode& mode)
{
	uint16_t currentSettings = getRegisterValue(registers::configuration) & 0xFFF8;//reset only changed setting

	setRegisterValue(registers::configuration, currentSettings | getEnumValue(mode));
}

double INA219::getShuntVoltageValue(void)
{
	return static_cast<int16_t>(getRegisterValue(registers::shuntVoltage)) * 0.01;
}

double INA219::getBusVoltageValue(void)
{
	return (getRegisterValue(registers::busVoltage) >> 3) * 0.004;
}

bool INA219::getConversionReadyState(void)
{
	return static_cast<bool>(getRegisterValue(registers::busVoltage) & 0x0002);
}

bool INA219::getMathOverflowFlag(void)
{
	return static_cast<bool>(getRegisterValue(registers::busVoltage) & 0x0001);
}

double INA219::getPowerValue(void)
{
	return getRegisterValue(registers::power) * m_currentLsb * 20.0;
}

double INA219::getCurrentValue(void)
{
	return static_cast<int16_t>(getRegisterValue(registers::current)) * m_currentLsb;
}
