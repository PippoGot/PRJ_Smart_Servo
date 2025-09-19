/*
 * MG996R.hpp
 *
 *  Created on: Aug 26, 2025
 *      Author: gotta
 */

#pragma once

#include "stm32f1xx_hal.h"

#include "i2c_slave.hpp"
#include "i2c_register.hpp"
#include "AS5600.hpp"
#include "control.hpp"
#include "DRV8870.hpp"
#include "CONFIG.h"


#define PI 3.14159265f


class MG996R {

public:

	// --- Constructor

	MG996R() = default;



	// --- Init Method

	void init(
			I2C_HandleTypeDef* external_i2c_handle,
			I2C_HandleTypeDef* internal_i2c_handle,
			TIM_HandleTypeDef* control_loop_timer_handle,
			TIM_HandleTypeDef* driver_timer_handle
		);


	// --- Control Loop

	void controlLoop();


	// --- Callbacks

	inline void desiredPosition(uint16_t position) { regActualPosition.typedSet(position); }
	inline uint16_t desiredPosition(void) { return regDesiredPosition.typedGet(); }

	inline uint16_t currentPosition() { return regActualPosition.typedGet(); }


	// Slave Manager
	I2C::Slave slave{_registerMap};


private:

	// --- Variables

	// Init flag
	bool _isInit = false;


	// Handles
	I2C_HandleTypeDef* _external_i2c_handle;
	I2C_HandleTypeDef* _internal_i2c_handle;
	TIM_HandleTypeDef* _control_loop_timer_handle;
	TIM_HandleTypeDef* _driver_timer_handle;


	// --- Registers
	// Map
	I2C::RegisterMap _registerMap;


	// Controller Parameters
	I2C::VirtualRegister<float> regProportionalGain{RegAddr::ProportionalGain, _registerMap};
	I2C::VirtualRegister<float> regIntegralGain{RegAddr::IntegralGain, _registerMap};
	I2C::VirtualRegister<float> regDerivativeGain{RegAddr::DerivativeGain, _registerMap};
	I2C::VirtualRegister<float> regOutputMax{RegAddr::OutputMax, _registerMap};
	I2C::VirtualRegister<float> regOutputLow{RegAddr::OutputMin, _registerMap};

	// Servo State
	I2C::VirtualRegister<uint16_t> regDesiredPosition{RegAddr::DesiredPosition, _registerMap};
	I2C::VirtualRegister<uint16_t> regActualPosition{RegAddr::ActualPosition, _registerMap};
	I2C::VirtualRegister<uint16_t> regMotorCurrent{RegAddr::MotorCurrent, _registerMap};
	I2C::VirtualRegister<uint16_t> regMotorVoltage{RegAddr::MotorVoltage, _registerMap};
	I2C::VirtualRegister<uint16_t> regMotorPower{RegAddr::MotorPower, _registerMap};
	I2C::VirtualRegister<uint16_t> regDutyCycle{RegAddr::DutyCycle, _registerMap};

	// Sensors Configuration
	I2C::VirtualRegister<uint16_t> regZeroPosition{RegAddr::ZeroPosition, _registerMap};
	I2C::VirtualRegister<uint16_t> regStopPosition{RegAddr::StopPosition, _registerMap};
	I2C::VirtualRegister<uint16_t> regCurrentCalibration{RegAddr::CurrentCalibration, _registerMap};

	// Slave Configuration
	I2C::VirtualRegister<uint8_t> regDeviceAddress{RegAddr::DeviceAddress, _registerMap};
	I2C::VirtualRegister<uint8_t> regDeviceConfiguration{RegAddr::DeviceConfiguration, _registerMap};


	// Components & Configs
	AS5600 _encoder{
		DefaultSettings::AS5600_Address,
		DefaultSettings::I2C_MaxDelay
	};

	SWPIDController _controller{
		DefaultSettings::TimeStep,
		DefaultSettings::ProportionalGain,
		DefaultSettings::IntegralGain,
		DefaultSettings::DerivativeGain
	};

	DRV8870 _driver{
		DefaultSettings::DRV8870_IN1,
		DefaultSettings::DRV8870_IN2,
		DefaultSettings::DRV8870_Steps
	};
};
