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
	slave.init(_external_i2c_handle, DefaultSettings::SlaveAddress);


	// Encoder setup
	_encoder.init(_internal_i2c_handle);

	_encoder.startPosition(DefaultSettings::AS5600_ZeroPos);
	_encoder.stopPosition(DefaultSettings::AS5600_StopPos);

	uint16_t initial_position;
	_encoder.digitalAngle(&initial_position);
	regActualPosition.typedSet(initial_position);
	regDesiredPosition.typedSet(initial_position);


	// Controller setup
	_controller.changeParameter(SWPID::ControllerParam::AntiwindupMax, DefaultSettings::AntiwindupMax);
	_controller.changeParameter(SWPID::ControllerParam::AntiwindupMin, DefaultSettings::AntiwindupMin);
	_controller.changeParameter(SWPID::ControllerParam::OutputMax, DefaultSettings::OutputMax);
	_controller.changeParameter(SWPID::ControllerParam::OutputMin, DefaultSettings::OutputMin);


	// Driver setup
	_driver.init(_driver_timer_handle);


	// Control loop timer start
	HAL_TIM_Base_Start_IT(_control_loop_timer_handle);


	// Bind callbacks
	bindCallbacks();


	// Toggle init flag
	_isInit = true;
}

void MG996R::bindCallbacks() {
	// Controller Parameters Callbacks
	regProportionalGain.bind([this]() {
		_controller.changeParameter(
				SWPID::ControllerParam::ProportionalGain,
				regProportionalGain.typedGet());
	});

	regIntegralGain.bind([this]() {
		_controller.changeParameter(
				SWPID::ControllerParam::IntegralGain,
				regIntegralGain.typedGet());
	});

	regDerivativeGain.bind([this]() {
		_controller.changeParameter(
				SWPID::ControllerParam::DerivativeGain,
				regDerivativeGain.typedGet());
	});

	regAntiwindupMax.bind([this]() {
		_controller.changeParameter(
				SWPID::ControllerParam::AntiwindupMax,
				regAntiwindupMax.typedGet());
	});

	regAntiwindupMin.bind([this]() {
		_controller.changeParameter(
				SWPID::ControllerParam::AntiwindupMin,
				regAntiwindupMin.typedGet());
	});

	regOutputMax.bind([this]() {
		_controller.changeParameter(
				SWPID::ControllerParam::OutputMax,
				regOutputMax.typedGet());
	});

	regOutputMin.bind([this]() {
		_controller.changeParameter(
				SWPID::ControllerParam::OutputMin,
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
	int32_t current_position;

	// Position reading (extend to sensor reading)
	status = _encoder.digitalAngle(&current_reading);

	if (status == HAL_OK) {
		current_position = _unwrapper.update(current_reading);
		regActualPosition.typedSet(current_position);
	}
	else {
		return;
	}

	// Compute control input
	float reference = regDesiredPosition.typedGet() * PI / 4096.0f;
	float feedback = regActualPosition.typedGet() * PI / 4096.0f;

	float u_PID = _controller.step(reference, feedback);
	regDutyCycle.typedSet(static_cast<int32_t>(1024 * u_PID));

	// Set driver duty cycle
	_driver.duty(-u_PID, DRV8870::StopMode::BRAKE);
}

