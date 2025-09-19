/**
 * @file i2c_device.hpp
 * @brief STM32 I2C device abstraction with thread-safe operations.
 *
 * This module provides a convenient wrapper for interacting with I2C devices
 * on STM32 microcontrollers using the HAL library. It supports:
 *   - Reading and writing arbitrary data types with byte::EndianType handling.
 *   - Masked register operations.
 *   - Thread-safe access using (optional) FreeRTOS mutexes.
 *   - Device presence detection and bus scanning.
 *
 * The I2C_Device class handles the low-level HAL calls, mutex management,
 * and address handling, making it easier to work with multiple I2C peripherals
 * in a safe and portable way.
 *
 * @author 	Pippogot
 * @date	21/08/2025
 */

#pragma once

#include <string>
#include <cstdio>
#include "i2c.h"

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#include "byte_utils.hpp"

namespace I2C {

	// ---------------------------------------------------- Bus Scan Utility Function ---
	/**
	 * @brief Scan the I2C bus for connected devices.
	 *
	 * Iterates through valid 7-bit addresses (0x08–0x77) and checks
	 * if a device responds using the Device wrapper.
	 *
	 * @param handle Pointer to the HAL I2C bus handle.
	 * @return std::string A string listing detected device addresses in hex format.
	 */
	std::string busScan(I2C_HandleTypeDef* handle);


	// --------------------------------------------------------- Generic Device Class ---
	/**
	 * @brief Generic I2C device interface for STM32 MCUs.
	 *
	 * Provides low-level I2C communication with optional thread safety.
	 * Supports reading/writing of arbitrary types (8-bit, 16-bit, etc.)
	 * using templates and the byte_utils helpers for endian-aware operations.
	 *
	 * An I2C Device should get the two important constructor parameters, which are
	 * device address and maximum delay for response.
	 * The I2C bus (I2C_HandleTypeDef*) should be passed through the .init() method.
	 *
	 *
	 * Subclass implementation guidelines:
	 *
	 * - Default Address (public)
	 * 		Preferably include a static constexpr DEFAULT_ADDRESS to use as default.
	 *
	 * - Register map (protected)
	 * 		A list with all accessible registers, for example:
	 * 			enum class Register : uint8_t {};
	 *
	 * - Read/Write method (either public or protected)
	 * 		Exclusively use the Device superclass method to interface with I2C bus.
	 *
	 * 		Apply any transformation to the data in the wrapper method.
	 * 		Use the classic signatures
	 *
	 * 			HAL_StatusTypeDef readingMethod(any* data_buffer);
	 * 			bool/HAL_StatusTypeDef writingMethod(any data);
	 *
	 * 		where the status is returned and the data is stored in a buffer for read
	 * 		operations, while a bool or the status result is returned for a write
	 * 		operaiton.
	 *
	 * - Configuration (either public or (preferably) protected)
	 * 		Prefer enum class for configurations management
	 *
	 * 			enum class ConfigMask : uint8_t {};
	 *
	 * 		for isolating configuration bits in a register, and
	 *
	 * 			enum class SomeConfig : uint8_t {};
	 *
	 * 		for every available configuration option.
	 *
	 * 		Use the enums when implementing the high level read and write methods
	 * 		to avoid type mismatching, magic numbers and improve code clarity.
	 *
	 * 		Preferably use the same name for read and write of the same configuration
	 * 		taking advantage of the overloading feature.
	 *
	 * - Constants & Modes (either public or (preferably) protected)
	 *  	If you need conversion constants or any constant in a general, and for
	 *  	specific use in the class, use static constexpr with suitable naming.
	 *  	For example:
	 *  		static constexpr float ADC_TO_DEG = 360.0f / 4096.0f;
	 *
	 * - Documentation & Code Clarity
	 * 		Include as much as possible, references to datasheet or snippets for
	 * 		code explanation from it.
	 *
	 * 		Match as much as possible naming conventions in the datasheet,
	 * 		e.g. register names or  configuration option names.
	 *
	 * - Code Implementation
	 * 		For simple one-line function such as straight-forward read and write
	 * 		with no other logic, prefer inline implementation using the available
	 * 		superclass methods.
	 *
	 * 		Group up methods and enums related to the same topic/configuration.
	 *
	 */
	class Device {
	public:
		/**
		 * @brief Construct an I2C device instance.
		 *
		 * @param address 7-bit device address (will be shifted internally).
		 * @param wait_delay Timeout for HAL operations (default: HAL_MAX_DELAY).
		 */
		Device(uint8_t address, uint32_t wait_delay = HAL_MAX_DELAY);

		/**
		 * @brief Destructor.
		 *
		 * Releases resources, including the FreeRTOS mutex if enabled.
		 */
		~Device();

		/// @brief Initialize the I2C device with HAL handle.
		inline void init(I2C_HandleTypeDef* handle) { _handle = handle; }

		/// @brief Get the HAL I2C handle used by this device.
		I2C_HandleTypeDef* handle() const { return _handle; }

		/// @brief Get the 7-bit device address.
		uint8_t address() const { return _address >> 1; }

		/// @brief Get the configured timeout/delay for I2C transactions.
		uint32_t waitDelay() const { return _wait_delay; }

		/**
		 * @brief Check if the device responds on the I2C bus.
		 *
		 * Thread-safe if FreeRTOS is enabled.
		 *
		 * @return true if device acknowledges, false otherwise.
		 */
		bool isConnected();

		/**
		 * @brief Read a value of arbitrary type from a register.
		 *
		 * Template supports uint8_t, uint16_t, or any type with size known at compile time.
		 *
		 * @tparam T Type of data to read.
		 * @param reg Register address to read from.
		 * @param data Pointer to store the read value.
		 * @param order byte::EndianType (BigEndian by default).
		 * @return HAL_StatusTypeDef HAL_OK if successful, error code otherwise.
		 */
		template<typename T>
		HAL_StatusTypeDef read(uint8_t reg, T* data, byte::EndianType order = byte::EndianType::BigEndian){
			// Acquire the bus
			HAL_StatusTypeDef status = acquireBus();
			if (status != HAL_OK) return status;

			// Read
			uint8_t buffer[sizeof(T)];
			status = HAL_I2C_Mem_Read(
					_handle,
					_address,
					reg,
					I2C_MEMADD_SIZE_8BIT,
					buffer,
					sizeof(T),
					_wait_delay
					);

			// Release the bus
			releaseBus();
			if (status != HAL_OK) return status;

			// Concatenate bytes
			*data = byte::concat<T>(buffer, order);
			return HAL_OK;
		}

		/**
		 * @brief Write a value of arbitrary type to a register.
		 *
		 * @tparam T Type of data to write.
		 * @param reg Register address to write to.
		 * @param data Value to write.
		 * @param order byte::EndianType (BigEndian by default).
		 * @return HAL_StatusTypeDef HAL_OK if successful, error code otherwise.
		 */
		template<typename T>
		HAL_StatusTypeDef write(uint8_t reg, T data, byte::EndianType order = byte::EndianType::BigEndian) {
			// Acquire the bus
			HAL_StatusTypeDef status = acquireBus();
			if (status != HAL_OK) return status;

			// Split bytes
			uint8_t buffer[sizeof(T)];
			byte::split<T>(data, buffer, order);

			// Write
			status = HAL_I2C_Mem_Write(
					_handle,
					_address,
					reg,
					I2C_MEMADD_SIZE_8BIT,
					buffer,
					sizeof(T),
					_wait_delay
					);

			// Release the bus
			releaseBus();
			return status;
		}

		/**
		 * @brief Read a masked value from a register.
		 *
		 * Only bits set in `mask` are returned; other bits are zeroed.
		 *
		 * @tparam T Type of data to read.
		 * @param reg Register address to read from.
		 * @param mask Bitmask to apply to the read value.
		 * @param data Pointer to store masked value.
		 * @param order byte::EndianType (BigEndian by default).
		 * @return HAL_StatusTypeDef HAL_OK if successful, error code otherwise.
		 */
		template<typename T>
		HAL_StatusTypeDef maskedRead(uint8_t reg, T mask, T* data, byte::EndianType order = byte::EndianType::BigEndian) {
			// Temporary variable
			T raw;

			// Read
			HAL_StatusTypeDef status = read<T>(reg, &raw, order);
			if (status != HAL_OK) return status;

			// Masking
			*data = raw & mask;

			// Return
			return HAL_OK;
		}

		/**
		 * @brief Write a masked value to a register.
		 *
		 * Only bits set in `mask` are modified; others remain unchanged.
		 *
		 * @tparam T Type of data to write.
		 * @param reg Register address to write to.
		 * @param mask Bitmask specifying which bits to modify.
		 * @param value Value to write (aligned to mask LSB).
		 * @param order byte::EndianType (BigEndian by default).
		 * @return HAL_StatusTypeDef HAL_OK if successful, error code otherwise.
		 */
		template<typename T>
		HAL_StatusTypeDef maskedWrite(uint8_t reg, T mask, T value, byte::EndianType order = byte::EndianType::BigEndian) {
			// Temporary variable
			T current;

			// Read
			HAL_StatusTypeDef status = read<T>(reg, &current, order);
			if (status != HAL_OK) return status;

			// Masking
			current = (current & ~mask) | (value & mask);

			// Write & Return
			return write<T>(reg, current, order);
		}

	private:
		I2C_HandleTypeDef* _handle = nullptr; 	///< Pointer to HAL I2C handle.
		const uint8_t _address;     			///< 8-bit device address (HAL expects shifted value).
		const uint32_t _wait_delay; 			///< Timeout for I2C operations.

	#ifdef USE_FREERTOS
		SemaphoreHandle_t _mutex; 				///< Mutex for thread-safe access if FreeRTOS is enabled.
	#endif

		/**
		 * @brief Acquire bus access (mutex) before performing I2C operation.
		 *
		 * @return HAL_StatusTypeDef HAL_OK if acquired, error otherwise.
		 */
		HAL_StatusTypeDef acquireBus();

		/**
		 * @brief Release the bus mutex after I2C operation.
		 */
		void releaseBus();
	};

}
