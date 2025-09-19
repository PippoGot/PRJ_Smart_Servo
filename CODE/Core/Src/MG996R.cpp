#include "MG996R.hpp"


void MG996R::init(
		I2C_HandleTypeDef* external_i2c_handle,
		I2C_HandleTypeDef* internal_i2c_handle,
		TIM_HandleTypeDef* control_loop_timer_handle,
		TIM_HandleTypeDef* driver_timer_handle
	) {
	// Handle storing
	_external_i2c_handle = external_i2c_handle;
	_internal_i2c_handle = internal_i2c_handle;
	_control_loop_timer_handle = control_loop_timer_handle;
	_driver_timer_handle = driver_timer_handle;


	// Slave setup
	slave.init(_external_i2c_handle);


	// Encoder setup
	_encoder.init(_internal_i2c_handle);

	_encoder.startPosition(DefaultSettings::AS5600_ZeroPos);
	_encoder.stopPosition(DefaultSettings::AS5600_StopPos);

	uint16_t initial_position;
	_encoder.digitalAngle(&initial_position);
	regActualPosition.typedSet(initial_position);
	regDesiredPosition.typedSet(initial_position);


	// Controller setup
	_controller.setOutputLimits(DefaultSettings::OutputMin, DefaultSettings::OutputMax);


	// Driver setup
	_driver.init(_driver_timer_handle);


	// Control loop timer start
	HAL_TIM_Base_Start_IT(_control_loop_timer_handle);


	// Toggle init flag
	_isInit = true;
}

void MG996R::controlLoop() {
	// Init guard
	if (!_isInit) return;

	// Loop variables
	HAL_StatusTypeDef status;
	uint16_t current_position;

	// Position reading (extend to sensor reading)
	status = _encoder.digitalAngle(&current_position);
	if (status == HAL_OK) {
		regActualPosition.typedSet(current_position);
	}
	else {
		return;
	}

	// Compute control input
	float u_PID = _controller.compute(regDesiredPosition.typedGet(), regActualPosition.typedGet(), PI / 4096.0f);

	// Clamp control input
	if (u_PID > 1.0f) u_PID = 1.0f;
	else if (u_PID < -1.0f) u_PID = -1.0f;

	// Set driver duty cycle
	_driver.duty(u_PID, DRV8870::StopMode::BRAKE);
}

