/**
 * @file AS5600.hpp
 * @brief Driver for the AMS AS5600 magnetic rotary position sensor.
 *
 * This device is a 12-bit angle sensor with I2C interface.
 * Registers include raw angle, scaled angle, configuration, etc.
 *
 * Reference: AS5600 datasheet (ams OSRAM).
 * https://look.ams-osram.com/m/7059eac7531a86fd/original/AS5600-DS000365.pdf
 *
 * @author 	Pippogot
 * @date	21/08/2025
 */

#pragma once

#include "i2c_device.hpp"


class AS5600 : public I2C::Device {

public:

	// --- Default I2C Address
	static constexpr uint8_t DEFAULT_ADDRESS = 0x36;

	// --- Constructor

	AS5600(uint8_t address = DEFAULT_ADDRESS, uint32_t wait_delay = HAL_MAX_DELAY)
		: I2C::Device(address, wait_delay) {}


	// --- Data Methods

	enum class DigitalAngle : uint8_t {
		RAW,
		NORMAL,
	};

	/**
	 * @brief Digital Angle Read
	 *
	 * The RAW_ANGLE register contains the 12-bit angle value.
	 * The scaled output value is available in the ANGLE register.
	 *
	 * NOTE: The ANGLE register has a 10-LSB hysteresis at the limit
	 * of the 360 degree range to avoid discontinuity points or toggling
	 * of the output within one rotation.
	 *
	 * To get get value without the dead zone use reading from RAW_ANGLE
	 * register.
	 */
	HAL_StatusTypeDef digitalAngle(uint16_t* buffer, DigitalAngle type = DigitalAngle::NORMAL);



	enum class AngleUnit : uint8_t {
		DEGREES = 0x00,
		RADIANS = 0x01,
	};

	/**
	 * @brief Processed Angle Read
	 *
	 * The digital angle is converted in physical units (either degrees
	 * or radians) and offset are applied (if any). Default unit is
	 * radians.
	 */
	HAL_StatusTypeDef analogAngle(float* buffer, AngleUnit unit);
	inline HAL_StatusTypeDef analogAngle(float* buffer) { return analogAngle(buffer, _default_unit); }

	/**
	 * @brief Change the default angle unit
	 */
	inline void defaultUnit(AngleUnit unit) { _default_unit = unit; }
	inline AngleUnit defaultUnit(void) { return _default_unit; }

	/**
	 * @brief Change the angle read offset
	 */
	inline void angleOffset(float offset) { _angle_offset = offset; }
	inline float angleOffset(void) { return _angle_offset; }


	// --- Configuration & Settings
	// (See data-sheet page 19, figure 22 for more details)

	/**
	 * @brief Updates the whole configuration register content with the given data
	 *
	 * TODO: better management of data, to avoid magic numbers
	 * and data mismatch
	 *
	 * NOTE 1: To change a configuration, read out the register, modify
	 * only the desired bits and write the new configuration.
	 *
	 * NOTE 2: During power-up, configuration registers are reset to the
	 * permanently programmed value. Not programmed bits are zero.
	 */
	inline bool configuration(uint16_t value) { return write<uint16_t>(static_cast<uint8_t>(Register::CONF_H), value) == HAL_OK; }
	inline HAL_StatusTypeDef configuration(uint16_t* buffer) { return read<uint16_t>(static_cast<uint8_t>(Register::CONF_H), buffer); }


	// Power Mode Configuration
	enum class PowerModeConfig : uint8_t {		// AS5600 power modes
		NOM 	= 0x00 << 0,					// 6.5 mA
		LPM1 	= 0x01 << 0,					// 3.5 mA, 5   ms polling time
		LPM2 	= 0x02 << 0,					// 1.8 mA, 20  ms polling time
		LPM3 	= 0x03 << 0,					// 1.5 mA, 100 ms polling time
	};

	/**
	 * @brief Power Mode Configuration
	 * (See data sheet page 31 for more details)
	 *
	 * A digital state machine automatically manages the low power
	 * modes to reduce the average current consumption. Three low
	 * power modes are available and can be enabled.
	 */
	inline bool powerMode(PowerModeConfig option) {
		return maskedWrite<uint8_t>(
				static_cast<uint8_t>(Register::CONF_L),
				static_cast<uint8_t>(ConfigMask::POWER_MODE),
				static_cast<uint8_t>(option)
				) == HAL_OK;
	}

	HAL_StatusTypeDef powerMode(PowerModeConfig* buffer);


	// Hysteresis Configuration
	enum class HysteresisConfig : uint8_t {		// AS5600 hysteresis value
		HYST_0 = 0x00 << 2,						// Off
		HYST_1 = 0x01 << 2,						// 1 LSB
		HYST_2 = 0x02 << 2,						// 2 LSBs
		HYST_3 = 0x03 << 2,						// 3 LSBs
	};

	/**
	 * @ brief Hysteresis Configuration
	 * (See data sheet page 31 for more details)
	 *
	 * To avoid any toggling of the output when the magnet is not
	 * moving, a 1 to 3 LSB hysteresis of the 12-bit resolution can be
	 * enabled.
	 */
	inline bool hysteresis(HysteresisConfig option) {
		return maskedWrite<uint8_t>(
				static_cast<uint8_t>(Register::CONF_L),
				static_cast<uint8_t>(ConfigMask::HYSTERESIS),
				static_cast<uint8_t>(option)
				) == HAL_OK;
	}

	HAL_StatusTypeDef hysteresis(HysteresisConfig* buffer);


	// Output Mode Configuration
	enum class OutputModeConfig : uint8_t {		// AS5600 output modes
		ANALOG_FULL_RANGE 		= 0x00 << 4,	// Analog output, 0% = GND - 100% = VDD
		ANALOG_REDUCED_RANGE	= 0x01 << 4,	// Analog output, 10% = GND - 90% = VDD
		DIGITAL_PWM 			= 0x02 << 4,	// Digital PWM output
	};

	/**
	 * @brief Output Stage Configuration
	 * (See data sheet page 25-27 for more details)
	 *
	 * This method is used to choose between an analog ratiometric
	 * output (default) and a digital PWM output. If PWM is selected,
	 * the DAC is powered down.
	 *
	 * Without regard to which output is enabled, an external
	 * unit can read the angle through I²C interface at any time.
	 */
	inline bool outputMode(OutputModeConfig option) {
		return maskedWrite<uint8_t>(
				static_cast<uint8_t>(Register::CONF_L),
				static_cast<uint8_t>(ConfigMask::OUTPUT_MODE),
				static_cast<uint8_t>(option)
				) == HAL_OK;
	}

	HAL_StatusTypeDef outputMode(OutputModeConfig* buffer);


	// PWM Frequency Configuration
	enum class PWMFrequencyConfig : uint8_t {	// AS5600 PWM frequency
		PWM_115Hz = 0x00 << 6,					// PWM frequency of 115Hz
		PWM_230Hz = 0x01 << 6,					// PWM frequency of 230Hz
		PWM_460Hz = 0x02 << 6,					// PWM frequency of 460Hz
		PWM_920Hz = 0x03 << 6,					// PWM frequency of 920Hz
	};

	inline bool pwmFrequency(PWMFrequencyConfig option) {
		return maskedWrite<uint8_t>(
				static_cast<uint8_t>(Register::CONF_L),
				static_cast<uint8_t>(ConfigMask::PWM_FREQUENCY),
				static_cast<uint8_t>(option)
				) == HAL_OK;
	}

	HAL_StatusTypeDef pwmFrequency(PWMFrequencyConfig* buffer);


	// Slow Filter Configuration
	enum class SlowFilterConfig : uint8_t {		// AS5600 slow filter value
		SLOW_FILTER_16x = 0x00 << 0,			// 16x
		SLOW_FILTER_8x 	= 0x01 << 0,			// 8x
		SLOW_FILTER_4x 	= 0x02 << 0,			// 4x
		SLOW_FILTER_2x 	= 0x03 << 0,			// 2x
	};

	/**
	 * @brief Step Response & Filter Settings
	 * (See data sheet page 28 for more details)
	 *
	 * The AS5600 has a digital post-processing programmable filter
	 * which can be set in fast or slow modes. The fast filter mode can
	 * be enabled by setting a fast filter threshold.
	 *
	 * If the fast filter is OFF, the step output response is controlled
	 * by the slow linear filter. The step response of the slow filter
	 * is programmable.
	 */
	inline bool slowFilter(SlowFilterConfig option) {
		return maskedWrite<uint8_t>(
				static_cast<uint8_t>(Register::CONF_H),
				static_cast<uint8_t>(ConfigMask::SLOW_FILTER),
				static_cast<uint8_t>(option)
				) == HAL_OK;
	}

	HAL_StatusTypeDef slowFilter(SlowFilterConfig* buffer);


	// Fast Filter Configuration
	enum class FastFilterConfig : uint8_t {		// AS5600 fast filter threshold value
		SLOW_FILTER_ONLY 	= 0x00 << 2,		// Slow filter only
		FAST_FILTER_6_LSBs 	= 0x01 << 2,		// 6 LSBs
		FAST_FILTER_7_LSBs 	= 0x02 << 2,		// 7 LSBs
		FAST_FILTER_9_LSBs	= 0x03 << 2,		// 9 LSBs
		FAST_FILTER_18_LSBs	= 0x04 << 2,		// 18 LSBs
		FAST_FILTER_21_LSBs = 0x05 << 2,		// 21 LSBs
		FAST_FILTER_24_LSBs = 0x06 << 2,		// 24 LSBs
		FAST_FILTER_10_LSBs = 0x07 << 2,		// 10 LSBs
	};

	/**
	 * @brief Fast Filter Threshold Configuration
	 * (See data sheet page 29 for more details)
	 *
	 * For a fast step response and low noise after settling, the fast
	 * filter can be enabled. The fast filter works only if the input
	 * variation is greater than the fast filter threshold, otherwise the
	 * output response is determined only by the slow filter.
	 */
	inline bool fastFilter(FastFilterConfig option) {
		return maskedWrite<uint8_t>(
				static_cast<uint8_t>(Register::CONF_H),
				static_cast<uint8_t>(ConfigMask::FAST_FILTER),
				static_cast<uint8_t>(option)
				) == HAL_OK;
	}

	HAL_StatusTypeDef fastFilter(FastFilterConfig* buffer);


	// Watchdog Configuration
	enum class WatchdogConfig : uint8_t {		// AS5600 watch-dog state
		WATCHDOG_OFF 	= 0x00 << 5,			// Watchdog off
		WATCHDOG_ON 	= 0x01 << 5,			// Watchdog on
	};

	/**
	 * @brief Watchdog Timer Configuration
	 * (See data sheet page 31 for more details)
	 *
	 * The watchdog timer allows saving power by switching into
	 * Low Power Mode 3 if the angle stays within the watchdog
	 * threshold of 4 LSB for at least one minute.
	 */
	inline bool watchdog(WatchdogConfig option) {
		return maskedWrite<uint8_t>(
				static_cast<uint8_t>(Register::CONF_H),
				static_cast<uint8_t>(ConfigMask::WATCHDOG),
				static_cast<uint8_t>(option)
				) == HAL_OK;
	}

	HAL_StatusTypeDef watchdog(WatchdogConfig* buffer);


	// Angle Sweep Configuration

	/**
	 * @brief Configure the start position (ZPOS) for narrower angular range
	 * (See data-sheet page 19 for more details)
	 *
	 * The angular range must be greater than 18 degrees.
	 * In case of narrowed angular range, the resolution is not scaled to
	 * narrowed range (e.g. 0° to 360°(full-turn) → 4096dec; 0° to 180° → 2048dec).
	 */
	inline bool startPosition(uint16_t value) { return write<uint16_t>(static_cast<uint8_t>(Register::ZPOS_H), value) == HAL_OK; }
	inline HAL_StatusTypeDef startPosition(uint16_t* buffer) { return read<uint16_t>(static_cast<uint8_t>(Register::ZPOS_H), buffer); }

	/**
	 * @brief Configure the stop position (MPOS) for a narrower angular range
	 * (See data-sheet page 19 for more details)
	 *
	 * The angular range must be greater than 18 degrees.
	 * In case of narrowed angular range, the resolution is not scaled to
	 * narrowed range (e.g. 0° to 360°(full-turn) → 4096dec; 0° to 180° → 2048dec).
	 */
	inline bool stopPosition(uint16_t value) { return write<uint16_t>(static_cast<uint8_t>(Register::MPOS_H), value) == HAL_OK; }
	inline HAL_StatusTypeDef stopPosition(uint16_t* buffer) { return read<uint16_t>(static_cast<uint8_t>(Register::MPOS_H), buffer); }

	/**
	 * @brief Configure the maximum angle (MANG) for a narrower angular range
	 * (See data-sheet page 19 for more details)
	 *
	 * The angular range must be greater than 18 degrees.
	 * In case of narrowed angular range, the resolution is not scaled to
	 * narrowed range (e.g. 0° to 360°(full-turn) → 4096dec; 0° to 180° → 2048dec).
	 */
	inline bool maxAngle(uint16_t value) { return write<uint16_t>(static_cast<uint8_t>(Register::MANG_H), value) == HAL_OK; }
	inline HAL_StatusTypeDef maxAngle(uint16_t* buffer) { return read<uint16_t>(static_cast<uint8_t>(Register::MANG_H), buffer); }


	// --- Utility & Misc

	// Helper masks to isolate specific bits of the STATUS register
	enum class MagnetStatus : uint8_t {
		STRONG		= 0x08,	// AGC minimum gain overflow, magnet too strong
		WEAK		= 0x10,	// AGC maximum gain overflow, magnet too weak
		DETECTED 	= 0x20,	// Magnet was detected
	};

	/**
	 * @brief Read the magnet status
	 * (See data-sheet page 20, table 23 and page 31 for more details)
	 *
	 * As a safety and diagnostic feature, the AS5600 indicates the
	 * absence of the magnet. If the measured magnet field strength
	 * goes below the minimum specified level (Bz_ERROR), the output
	 * is driven low, without regard to which output mode has been
	 * selected (analog or PWM) and the MD bit in the STATUS
	 * register is 0.
	 */
	HAL_StatusTypeDef magnetStatus(MagnetStatus* buffer);

	bool magnetDetected(void);
	bool magnetWeak(void);
	bool magnetStrong(void);


	/**
	 * @brief Automatic Gain Control (AGC) Read
	 * (See data-sheet page 20 for more details)
	 *
	 * The AS5600 uses Automatic Gain Control in a closed loop to
	 * compensate for variations of the magnetic field strength due
	 * to changes of temperature, airgap between IC and magnet, and
	 * magnet degradation. The AGC register indicates the gain.
	 * For the most robust performance, the gain value should be in the
	 * center of its range. The airgap of the physical system can be
	 * adjusted to achieve this value.
	 *
	 * In 5V operation, the AGC range is 0-255 counts. The AGC range
	 * is reduced to 0-128 counts in 3.3V mode.
	 */
	inline HAL_StatusTypeDef gainValue(uint8_t* buffer) { return read<uint8_t>(static_cast<uint8_t>(Register::AGC), buffer); }


	/**
	 * @brief Internal CORDIC Magnitude Read
	 * (See data-sheet page 20 for more details)
	 *
	 * The MAGNITUDE register indicates the magnitude value of the
	 * internal CORDIC.
	 */
	inline HAL_StatusTypeDef cordicMagnitude(uint16_t* buffer) { return read<uint16_t>(static_cast<uint8_t>(Register::MAGNITUDE_H), buffer); }


	/**
	 * @brief Used Angle Programming Read
	 * (See data-sheet page 21 for more details)
	 *
	 * The host microcontroller can perform a permanent programming
	 * of start (ZPOS) and stop (MPOS) position with a BURN_ANGLE command.
	 * The BURN_ANGLE command can be executed up to 3 times.
	 * This method shows how many times ZPOS and MPOS have been
	 * permanently written.
	 */
	inline HAL_StatusTypeDef usedAngleBurn(uint8_t* buffer) { return read<uint8_t>(static_cast<uint8_t>(Register::ZMCO), buffer); }


protected:

	// --- Variables
	AngleUnit _default_unit = AngleUnit::RADIANS;
	float _angle_offset = 0.0f;

	// --- Register Map (See data-sheet page 18, figure 21 for more details)
	// Addresses for each available register
	enum class Register : uint8_t {
		// Configuration Registers
		ZMCO 	= 0x00,			// [][][][][][][ZMCO(1:0)]

		ZPOS_H 	= 0x01,			// [][][][][ZPOS(11:8)]
		ZPOS_L 	= 0x02,			// [ZPOS(7:0)]

		MPOS_H 	= 0x03,			// [][][][][MPOS(11:8)]
		MPOS_L 	= 0x04,			// [MPOS(7:0)]

		MANG_H 	= 0x05,			// [][][][][MANG(11:8)]
		MANG_L 	= 0x06,			// [MANG(7:0)]

		CONF_H 	= 0x07,			// [][][WD][FTH(2:0)][SF(1:0)]
		CONF_L 	= 0x08,			// [PWMF(1:0)][OUTS(1:0)][HYST(1:0)][PM(1:0)]

		// Status Registers
		STATUS 		= 0x0B,		// [][][MD][ML][MH][][][]

		AGC 		= 0x1A,		// [AGC(7:0)]

		MAGNITUDE_H = 0x1B,		// [][][][][MAGNITUDE(11:8)]
		MAGNITUDE_L	= 0x1C, 	// [MAGNITUDE(7:0)]

		// Data Registers
		RAW_ANGLE_H = 0x0C,		// [][][][][RAW ANGLE(11:8)]
		RAW_ANGLE_L = 0x0D, 	// [RAW ANGLE(7:0)]

		ANGLE_H 	= 0x0E,		// [][][][][ANGLE(11:8)]
		ANGLE_L 	= 0x0F, 	// [ANGLE(7:0)]

		// BURN Register is not reported
	};


	// --- Masks

	// Configuration Bits (See data-sheet page 19, table 22 for more details)
	// Helper masks to isolate specific bits of the CONFIG register
	enum class ConfigMask : uint8_t {
		POWER_MODE    	= 0x03,
		HYSTERESIS    	= 0x0C,
		OUTPUT_MODE   	= 0x30,
		PWM_FREQUENCY 	= 0xC0,
		SLOW_FILTER   	= 0x03,
		FAST_FILTER		= 0x1C,
		WATCHDOG     	= 0x20,
	};


	// --- Conversion & Constants

	static constexpr float PI = 3.14159265359f;

	static constexpr float ADC_TO_DEG = 360.0f / 4096.0f;
	static constexpr float DEG_TO_ADC = 4096.0f / 360.0f;

	static constexpr float ADC_TO_RAD = (PI * 2.0f) / 4096.0f;
	static constexpr float RAD_TO_ADC = 4096.0f / (PI * 2.0f);
};
