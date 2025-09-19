#pragma once

#include "i2c.h"
#include <cstring>
#include <vector>

#include "i2c_register.hpp"

namespace I2C {

	class Slave {
	public:
		Slave(RegisterMap& regMap) :
			_handle(nullptr),
			_regMap(regMap),
			_currentRegisterAddr(0), _currentRegister(nullptr)
		{}

		void init(I2C_HandleTypeDef *handle) {
			_handle = handle;
			HAL_I2C_EnableListen_IT(_handle);
		}

		void onAddrMatch(uint8_t transferDirection) {
			if (!_handle) return;

			if (transferDirection == I2C_DIRECTION_TRANSMIT) {
				// Master → Slave

				HAL_I2C_Slave_Seq_Receive_IT(
						_handle,
						&_currentRegisterAddr,
						1,
						I2C_FIRST_FRAME
						);
			}
			else {
				// Master ← Slave
				_currentRegister = _regMap.access(_currentRegisterAddr);

				if (_currentRegister) {
					HAL_I2C_Slave_Seq_Transmit_IT(
						_handle,
						_currentRegister -> content,
						_currentRegister -> size(),
						I2C_FIRST_AND_LAST_FRAME
					);
				}
			}
		}

		void onReceiveComplete() {
			if (!_currentRegister) {
				_currentRegister = _regMap.access(_currentRegisterAddr);
				if (_currentRegister) {
					HAL_I2C_Slave_Seq_Receive_IT(
						_handle,
						_currentRegister -> content,
						_currentRegister -> size(),
						I2C_FIRST_AND_LAST_FRAME
					);
				}
			}
			else {
				processReceivedData();
			}

			_currentRegister = nullptr;
		}

		void onTransmitComplete() {
			_currentRegister = nullptr;
		}

		void onListenComplete() {
			HAL_I2C_EnableListen_IT(_handle);
		}

		void onError() {
			uint32_t errorCode = HAL_I2C_GetError(_handle);

			// AF ERROR
			if (errorCode == HAL_I2C_ERROR_AF) {

			}

			// BUS ERROR
			else if (errorCode == HAL_I2C_ERROR_BERR) {
				HAL_I2C_DeInit(_handle);
				HAL_I2C_Init(_handle);
			}

			HAL_I2C_EnableListen_IT(_handle);
		}

	private:
		I2C_HandleTypeDef* _handle;

		RegisterMap& _regMap;

		uint8_t _currentRegisterAddr;
		IVirtualRegister* _currentRegister;

		void processReceivedData() {
			if (!_currentRegister) return;
		}
	};

}
