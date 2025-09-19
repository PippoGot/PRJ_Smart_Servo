#include "i2c_device.hpp"

namespace I2C {

	std::string busScan(I2C_HandleTypeDef* handle) {
		std::string result;

		for(uint8_t addr = 0x08; addr <= 0x77; ++addr) {
			// Dummy Device
			Device dev(addr);
			dev.init(handle);

			// Address Buffer
			char buf[3];

			// Check Connection
			if(dev.isConnected()) {
				// Device responds, print address in uppercase hex
				snprintf(buf, sizeof(buf), "%02X", addr);
			} else {
				// No response, mark with "--"
				snprintf(buf, sizeof(buf), "--");
			}

			// Build Result String
			result += buf;
			if(addr != 0x77) result += " "; // space between addresses
		}

		// Return
		return result;
	}


	Device::Device(uint8_t address, uint32_t wait_delay)
		: _address(address << 1), _wait_delay(wait_delay)
	{
	#ifdef USE_FREERTOS
		_mutex = xSemaphoreCreateMutex();
	#endif
	}

	Device::~Device() {
	#ifdef USE_FREERTOS
		if(_mutex) {
			vSemaphoreDelete(_mutex);
			_mutex = nullptr;
		}
	#endif
	}

	HAL_StatusTypeDef Device::acquireBus() {
		if (!_handle) return HAL_ERROR;

	#ifdef USE_FREERTOS
		if(_mutex == nullptr) return HAL_ERROR;
		if(xSemaphoreTake(_mutex, _wait_delay) != pdTRUE) return HAL_TIMEOUT;
	#endif

		return HAL_OK;
	}

	void Device::releaseBus() {
	#ifdef USE_FREERTOS
		if(_mutex) xSemaphoreGive(_mutex);
	#endif
	}

	bool Device::isConnected() {
		HAL_StatusTypeDef status = acquireBus();
		if(status != HAL_OK) return false;

		// HAL helper: checks if device responds to a single trial
		status = HAL_I2C_IsDeviceReady(_handle, _address, 1, _wait_delay);

		releaseBus();
		return (status == HAL_OK);
	}

}
