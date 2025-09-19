#include "AS5600.hpp"

// --- Data

// Angle Reading
HAL_StatusTypeDef AS5600::digitalAngle(uint16_t* buffer, AS5600::DigitalAngle type) {
	switch (type) {
		case DigitalAngle::NORMAL:
			return read<uint16_t>(static_cast<uint8_t>(Register::ANGLE_H), buffer);
		case DigitalAngle::RAW:
			return read<uint16_t>(static_cast<uint8_t>(Register::RAW_ANGLE_H), buffer);
	}

	return HAL_ERROR;
}

HAL_StatusTypeDef AS5600::analogAngle(float* buffer, AS5600::AngleUnit unit) {
	uint16_t digital_buffer = 0;
	HAL_StatusTypeDef status = digitalAngle(&digital_buffer);

	switch (unit) {
		case AngleUnit::DEGREES:
			*buffer = static_cast<float>(digital_buffer) * ADC_TO_DEG;
			break;
		case AngleUnit::RADIANS:
			*buffer = static_cast<float>(digital_buffer) * ADC_TO_RAD;
			break;
	}

	*buffer += _angle_offset;

	return status;
}


// --- Configuration & Settings (See data-sheet page 19, figure 22 for more details)

HAL_StatusTypeDef AS5600::powerMode(AS5600::PowerModeConfig* buffer) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
			static_cast<uint8_t>(Register::CONF_L),
			static_cast<uint8_t>(ConfigMask::POWER_MODE),
			&data
			);

	*buffer = static_cast<PowerModeConfig>(data);

	return status;
}

HAL_StatusTypeDef AS5600::hysteresis(AS5600::HysteresisConfig* buffer) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
			static_cast<uint8_t>(Register::CONF_L),
			static_cast<uint8_t>(ConfigMask::HYSTERESIS),
			&data
			);

	*buffer = static_cast<HysteresisConfig>(data);

	return status;
}

HAL_StatusTypeDef AS5600::outputMode(AS5600::OutputModeConfig* buffer) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
			static_cast<uint8_t>(Register::CONF_L),
			static_cast<uint8_t>(ConfigMask::OUTPUT_MODE),
			&data
			);

	*buffer = static_cast<OutputModeConfig>(data);

	return status;
}

HAL_StatusTypeDef AS5600::pwmFrequency(AS5600::PWMFrequencyConfig* buffer) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
			static_cast<uint8_t>(Register::CONF_L),
			static_cast<uint8_t>(ConfigMask::PWM_FREQUENCY),
			&data
			);

	*buffer = static_cast<PWMFrequencyConfig>(data);

	return status;
}

HAL_StatusTypeDef AS5600::slowFilter(AS5600::SlowFilterConfig* buffer) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
			static_cast<uint8_t>(Register::CONF_H),
			static_cast<uint8_t>(ConfigMask::SLOW_FILTER),
			&data
			);

	*buffer = static_cast<SlowFilterConfig>(data);

	return status;
}

HAL_StatusTypeDef AS5600::fastFilter(AS5600::FastFilterConfig* buffer) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
			static_cast<uint8_t>(Register::CONF_H),
			static_cast<uint8_t>(ConfigMask::FAST_FILTER),
			&data
			);

	*buffer = static_cast<FastFilterConfig>(data);

	return status;
}

HAL_StatusTypeDef AS5600::watchdog(AS5600::WatchdogConfig* buffer) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
			static_cast<uint8_t>(Register::CONF_H),
			static_cast<uint8_t>(ConfigMask::WATCHDOG),
			&data
			);

	*buffer = static_cast<WatchdogConfig>(data);

	return status;
}


// --- Utility & Misc

// Magnet Status (See data-sheet page 20, table 23 for more details)
bool AS5600::magnetDetected(void) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
		static_cast<uint8_t>(Register::STATUS),
		static_cast<uint8_t>(MagnetStatus::DETECTED),
		&data
		);

	return (MagnetStatus::DETECTED == static_cast<MagnetStatus>(data)) && (HAL_OK == status);
}

bool AS5600::magnetWeak(void) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
		static_cast<uint8_t>(Register::STATUS),
		static_cast<uint8_t>(MagnetStatus::WEAK),
		&data
		);

	return (MagnetStatus::WEAK == static_cast<MagnetStatus>(data)) && (HAL_OK == status);
}

bool AS5600::magnetStrong(void) {
	uint8_t data;

	HAL_StatusTypeDef status = maskedRead<uint8_t>(
		static_cast<uint8_t>(Register::STATUS),
		static_cast<uint8_t>(MagnetStatus::STRONG),
		&data
		);

	return (MagnetStatus::STRONG == static_cast<MagnetStatus>(data)) && (HAL_OK == status);
}

HAL_StatusTypeDef AS5600::magnetStatus(AS5600::MagnetStatus* buffer) {
	uint8_t data;

	HAL_StatusTypeDef status = read<uint8_t>(static_cast<uint8_t>(Register::STATUS), &data);

	*buffer = static_cast<MagnetStatus>(data);

	return status;
}

