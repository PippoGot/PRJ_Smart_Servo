/**
 * @file DRV8870.hpp
 * @brief Driver for the TI DRV8870 brushed DC motor driver.
 *
 * This device is an H-bridge based motor driver with PWM interface.
 * It features integrated current regulation and is fully protected
 * from faults and short circuits, including under-voltage (UVLO),
 * over-current (OCP) and over-temperature (TSD).
 *
 * Reference: DRV8870 datasheet (Texas Instrument).
 * https://www.ti.com/lit/ds/symlink/drv8870.pdf?ts=1755879358407
 *
 * @author 	Pippogot
 * @date 	23/08/2025
 */

#pragma once

#include "tim.h"


class DRV8870 {

public:

	// --- Stop mode options

	enum class StopMode : uint8_t {
		COAST,
		BRAKE,
	};


	// --- Constructor

	DRV8870(uint32_t in1_channel, uint32_t in2_channel,	uint16_t pwm_steps)
		: _in1_channel(in1_channel), _in2_channel(in2_channel),	_pwm_steps(pwm_steps) {}


	// --- Core Methods

	/**
	 * @brief Initialize the device by passing the timer handle
	 *
	 * When initialized, set the duty cycle to 0.
	 */
	void init(TIM_HandleTypeDef* handle);

	/**
	 * @brief Sets a duty cycle with the specified logic
	 *
	 * In StopMode::BRAKE, during the off period, both inputs are high.
	 * In StopMode::COAST, during the off period, both inputs are low.
	 *
	 * According to the datasheet, BRAKE mode is recommended when using PWM.
	 */
	void duty(float value, StopMode mode);
	inline void duty(float value) { duty(value, _default_stop_mode); }

	/**
	 * @brief Stops the driver, setting the duty cycle to 0
	 */
	void stop(StopMode mode);
	inline void stop(void) { stop(_default_stop_mode); }

	/**
	 * @brief Change the default stop mode
	 */
	inline void defaultStopMode(StopMode mode) { _default_stop_mode = mode; }
	inline StopMode defaultStopMode(void) { return _default_stop_mode; }

protected:

	// --- Variables

	TIM_HandleTypeDef* _handle = nullptr;

	uint32_t _in1_channel;
	uint32_t _in2_channel;

	uint16_t _pwm_steps;

	float _min_duty = 1.0 / _pwm_steps;

	StopMode _default_stop_mode = StopMode::BRAKE;

};
