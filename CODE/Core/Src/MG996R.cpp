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


	// Controller setup
	_controller.changeParameter(SwitchingPID::ControllerParam::AntiwindupMax, DefaultSettings::AntiwindupMax);
	_controller.changeParameter(SwitchingPID::ControllerParam::AntiwindupMin, DefaultSettings::AntiwindupMin);
	_controller.changeParameter(SwitchingPID::ControllerParam::OutputMax, DefaultSettings::OutputMax);
	_controller.changeParameter(SwitchingPID::ControllerParam::OutputMin, DefaultSettings::OutputMin);


	// Driver setup
	_driver.init(_driver_timer_handle);
	_driver.stop();


	// Encoder setup
	_encoder.init(_internal_i2c_handle);

	bool status;
	status = _encoder.startPosition(DefaultSettings::AS5600_ZeroPos);
	status = _encoder.stopPosition(DefaultSettings::AS5600_StopPos);

	regPositionOffset.typedSet(DefaultSettings::AS5600_Offset);
	regDesiredPosition.typedSet(0);


	// Slave setup
	bindCallbacks();
	slave.init(_external_i2c_handle, DefaultSettings::SlaveAddress);


	// Control loop timer start
	HAL_TIM_Base_Start_IT(_control_loop_timer_handle);


	// Toggle init flag
	_isInit = true;
}

void MG996R::bindCallbacks() {
	// Controller Parameters Callbacks
	regProportionalGain.bind([this]() {
		_controller.changeParameter(
				SwitchingPID::ControllerParam::ProportionalGain,
				regProportionalGain.typedGet());
	});

	regIntegralGain.bind([this]() {
		_controller.changeParameter(
				SwitchingPID::ControllerParam::IntegralGain,
				regIntegralGain.typedGet());
	});

	regDerivativeGain.bind([this]() {
		_controller.changeParameter(
				SwitchingPID::ControllerParam::DerivativeGain,
				regDerivativeGain.typedGet());
	});

	regAntiwindupMax.bind([this]() {
		_controller.changeParameter(
				SwitchingPID::ControllerParam::AntiwindupMax,
				regAntiwindupMax.typedGet());
	});

	regAntiwindupMin.bind([this]() {
		_controller.changeParameter(
				SwitchingPID::ControllerParam::AntiwindupMin,
				regAntiwindupMin.typedGet());
	});

	regOutputMax.bind([this]() {
		_controller.changeParameter(
				SwitchingPID::ControllerParam::OutputMax,
				regOutputMax.typedGet());
	});

	regOutputMin.bind([this]() {
		_controller.changeParameter(
				SwitchingPID::ControllerParam::OutputMin,
				regOutputMin.typedGet());
	});

	// Encoder Configuration Callbacks
	regZeroPosition.bind([this]() {	_encoder.startPosition(regZeroPosition.typedGet());	});
	regStopPosition.bind([this]() {	_encoder.stopPosition(regStopPosition.typedGet()); });

	// Slave Configuration
	regDeviceAddress.bind([this]() { slave.requestAddressChange(regDeviceAddress.typedGet()); });
}

void MG996R::controlLoop() {
	// Init guard
	if (!_isInit) return;

	// Loop variables
	HAL_StatusTypeDef status;
	uint16_t current_reading;
	//int32_t current_position;

	// Position reading (extend to sensor reading)
	status = _encoder.digitalAngle(&current_reading);

	if (status == HAL_OK) {
		//current_position = _unwrapper.update(current_reading);
		regActualPosition.typedSet(static_cast<int32_t>(current_reading) + regPositionOffset.typedGet());
	}
	else {
		return;
	}

	// Compute control input
	float reference = regDesiredPosition.typedGet() * mth::PI / 4096.0f;
	float feedback = regActualPosition.typedGet() * mth::PI / 4096.0f;

	float u_PID = _controller.step(reference, feedback);
	regDutyCycle.typedSet(static_cast<int32_t>(1024 * u_PID));

	// Set driver duty cycle
	_driver.duty(u_PID, DRV8870::StopMode::BRAKE);
}

