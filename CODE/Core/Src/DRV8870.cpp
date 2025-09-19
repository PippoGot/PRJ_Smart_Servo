#include "DRV8870.hpp"

// --- Core Methods

void DRV8870::init(TIM_HandleTypeDef* htim) {
	_handle = htim;

	HAL_TIM_PWM_Start(_handle, _in1_channel);
	HAL_TIM_PWM_Start(_handle, _in2_channel);

	stop(StopMode::BRAKE);
}

void DRV8870::duty(float duty, StopMode mode) {
	// Guard for initialization
	 if (!_handle) return;

	// Get the direction and magnitude of the duty cycle
	bool direction = (duty > 0.0f);
	duty = (direction ? duty : -duty);

	// If duty cycle is lower than minimum value, set to 0
	if (_min_duty > duty) {
		stop(mode);
		return;
	}

	// If duty cycle is greater than 1, set to 1
	if (1 < duty) duty = 1.0f;

	// Convert the duty cycle magnitude to a timer register value
	uint16_t step = static_cast<uint16_t>(_pwm_steps*duty);

	switch (mode) {
		case StopMode::BRAKE:
			// In BRAKE mode one of the two pins is always high
			if (direction) {
				__HAL_TIM_SET_COMPARE(_handle, _in1_channel, _pwm_steps);
				__HAL_TIM_SET_COMPARE(_handle, _in2_channel, _pwm_steps - step);
			}
			else {
				__HAL_TIM_SET_COMPARE(_handle, _in1_channel, _pwm_steps - step);
				__HAL_TIM_SET_COMPARE(_handle, _in2_channel, _pwm_steps);
			}
			break;
		case StopMode::COAST:
			// In COAST mode one of the two pins is always low
			if (direction) {
				__HAL_TIM_SET_COMPARE(_handle, _in1_channel, step);
				__HAL_TIM_SET_COMPARE(_handle, _in2_channel, 0);
			}
			else {
				__HAL_TIM_SET_COMPARE(_handle, _in1_channel, 0);
				__HAL_TIM_SET_COMPARE(_handle, _in2_channel, step);
			}
			break;
	}
}

void DRV8870::stop(StopMode mode) {
	// Guard for initialization
	if (!_handle) return;

	// Both pins high or low based on stop mode
	if (StopMode::BRAKE == mode) {
		__HAL_TIM_SET_COMPARE(_handle, _in1_channel, _pwm_steps);
		__HAL_TIM_SET_COMPARE(_handle, _in2_channel, _pwm_steps);
	}
	else {
		__HAL_TIM_SET_COMPARE(_handle, _in1_channel, 0);
		__HAL_TIM_SET_COMPARE(_handle, _in2_channel, 0);
	}
}
