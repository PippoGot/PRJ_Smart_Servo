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


	// --- Callbacks

	void bindCallbacks();

	// --- Control Loop

	void controlLoop();



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
	I2C::VirtualRegister<float> regAntiwindupMax{RegAddr::AntiwindupMax, _registerMap};
	I2C::VirtualRegister<float> regAntiwindupMin{RegAddr::AntiwindupMin, _registerMap};
	I2C::VirtualRegister<float> regOutputMax{RegAddr::OutputMax, _registerMap};
	I2C::VirtualRegister<float> regOutputMin{RegAddr::OutputMin, _registerMap};

	// Servo State
	I2C::VirtualRegister<int32_t> regDesiredPosition{RegAddr::DesiredPosition, _registerMap};
	I2C::VirtualRegister<int32_t> regActualPosition{RegAddr::ActualPosition, _registerMap, I2C::RegisterMode::ReadOnly};
	I2C::VirtualRegister<int32_t> regPositionOffset{RegAddr::PositionOffset, _registerMap};
	I2C::VirtualRegister<uint16_t> regMotorCurrent{RegAddr::MotorCurrent, _registerMap, I2C::RegisterMode::ReadOnly};
	I2C::VirtualRegister<uint16_t> regMotorVoltage{RegAddr::MotorVoltage, _registerMap, I2C::RegisterMode::ReadOnly};
	I2C::VirtualRegister<uint16_t> regMotorPower{RegAddr::MotorPower, _registerMap, I2C::RegisterMode::ReadOnly};
	I2C::VirtualRegister<int32_t> regDutyCycle{RegAddr::DutyCycle, _registerMap};

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

	AngleUnwrapper _unwrapper;

	SwitchingPID _controller{
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
