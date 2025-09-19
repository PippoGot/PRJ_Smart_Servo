/*
 * CONFIG.h
 *
 *  File containing all the system parameters and configuration data.
 *
 */

#pragma once

#include "stm32f1xx_hal.h"

#include "DRV8870.hpp"
#include "AS5600.hpp"
#include "control.hpp"
#include "i2c_slave.hpp"


namespace RegAddr {
	// SW-PID Controller
	constexpr uint8_t ProportionalGain 		= 0x00;
	constexpr uint8_t IntegralGain 			= 0x01;
	constexpr uint8_t DerivativeGain 		= 0x02;
	constexpr uint8_t OutputMax 			= 0x03;
	constexpr uint8_t OutputMin 			= 0x04;

	// Servo Motor State
	constexpr uint8_t DesiredPosition 		= 0x20;
	constexpr uint8_t ActualPosition 		= 0x21;
	constexpr uint8_t MotorCurrent	 		= 0x22;
	constexpr uint8_t MotorVoltage 			= 0x23;
	constexpr uint8_t MotorPower 			= 0x24;
	constexpr uint8_t DutyCycle 			= 0x25;

	// AS5600 Position Sensor
	constexpr uint8_t ZeroPosition 			= 0x40;
	constexpr uint8_t StopPosition 			= 0x41;

	// INA219 Current Sensor
	constexpr uint8_t CurrentCalibration 	= 0x42;

	// Slave Configuraiton
	constexpr uint8_t DeviceAddress 		= 0xF0;
	constexpr uint8_t DeviceConfiguration 	= 0xFF;
}

namespace DefaultSettings {
	// SW-PID Controller
	constexpr float TimeStep 				= 1e-3f;
	constexpr float ProportionalGain 		= 10.0f;
	constexpr float IntegralGain 			= 0.9f;
	constexpr float DerivativeGain 			= 0.1f;
	constexpr float OutputMax 				= 1.0f;
	constexpr float OutputMin 				= -1.0f;

	// DRV8870 Driver
	constexpr uint16_t DRV8870_Steps 		= 1024;
	constexpr uint32_t DRV8870_IN1			= TIM_CHANNEL_1;
	constexpr uint32_t DRV8870_IN2			= TIM_CHANNEL_2;

	// General I2C
	constexpr uint32_t I2C_MaxDelay			= 1;

	// AS5600 Position Sensor
	constexpr uint8_t AS5600_Address 		= 0x36;
	constexpr uint16_t AS5600_ZeroPos 		= 0x000;
	constexpr uint16_t AS5600_StopPos 		= 0xFFF;

	// INA219 Current Sensor
	constexpr uint8_t INA219_Address 		= 0x40;
	constexpr uint16_t INA219_Calibration 	= 0x0000;
}


