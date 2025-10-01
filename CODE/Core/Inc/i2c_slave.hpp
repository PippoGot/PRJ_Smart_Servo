#pragma once

#include "i2c.h"
#include <cstring>

#include "i2c_register.hpp"

namespace I2C {

	class Slave {
	public:
		static constexpr size_t MAX_REGISTER_SIZE = 32;


		Slave(RegisterMap& regMap) :
			_handle(nullptr),
			_regMap(regMap),
			_currentRegisterAddr(0), _currentRegister(nullptr),
			_pending_address(0)
		{
			std::memset(_rxBuffer, 0, sizeof(_rxBuffer));
		}


		void init(I2C_HandleTypeDef *handle) {
			_handle = handle;
			HAL_I2C_EnableListen_IT(_handle);
		}

		HAL_StatusTypeDef init(I2C_HandleTypeDef *handle, uint8_t slave_address) {
			if (!handle) return HAL_ERROR;
			_handle = handle;

			requestAddressChange(slave_address);
			applyPendingAddress();

			return HAL_OK;
		}


		void requestAddressChange(uint8_t new_address) {
			_pending_address = new_address;
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
						_currentRegister -> rawRead(),
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
					size_t regSize = _currentRegister -> size();
					if (regSize > MAX_REGISTER_SIZE) regSize = MAX_REGISTER_SIZE;

					HAL_I2C_Slave_Seq_Receive_IT(
						_handle,
						//_currentRegister -> content,
						_rxBuffer,
						regSize,
						I2C_FIRST_AND_LAST_FRAME
					);
				}
			}
			else {
				processReceivedData();
				_currentRegister = nullptr;
			}
		}

		void onTransmitComplete() {
			_currentRegister = nullptr;
			applyPendingAddress();
			HAL_I2C_EnableListen_IT(_handle);
		}

		void onListenComplete() {
			applyPendingAddress();
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

			applyPendingAddress();
			HAL_I2C_EnableListen_IT(_handle);
		}

	private:
		I2C_HandleTypeDef* _handle;

		RegisterMap& _regMap;

		uint8_t _currentRegisterAddr;
		IVirtualRegister* _currentRegister;
		uint8_t _pending_address;

		uint8_t _rxBuffer[MAX_REGISTER_SIZE];

		void processReceivedData() {
			if (!_currentRegister) return;

			_currentRegister -> rawWrite(_rxBuffer, _currentRegister -> size());
		}

		void applyPendingAddress() {
			if (_pending_address == 0 || !_handle) return;

			_handle -> Init.OwnAddress1 = _pending_address << 1;

			if (HAL_I2C_DeInit(_handle) == HAL_OK && HAL_I2C_Init(_handle) == HAL_OK) {
				HAL_I2C_EnableListen_IT(_handle);
			}

			_pending_address = 0;
		}
	};

}
