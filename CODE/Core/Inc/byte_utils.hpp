/**
 * @file byte_utils.hpp
 * @brief Utilities for byte manipulation.
 *
 * Provides helper functions for low-level data operations:
 *  - Combining and splitting bytes
 *  - Handling multi-byte endian type
 *
 * Default EndianType is BigEndian; LittleEndian is also supported.
 *
 * @author 	Pippogot
 * @date	19/08/2025
 */

#pragma once

#include <stdint.h>

namespace byte {

	// ------------------------------------------------------- Endian Type Enum Class ---
	/**
	 * @brief Defines byte order.
	 */
	enum class EndianType {
		BigEndian,
		LittleEndian
	};


	// -------------------------------------------------------------- Byte Operations ---
	/**
	 * @brief Combines an array of bytes into an integer value.
	 *
	 * @tparam T Integer type (e.g., uint16_t, uint32_t, uint64_t).
	 * @param bytes Pointer to the byte array.
	 * @param order Byte order to use (default: BigEndian).
	 * @return The combined integer value.
	 */
	template<typename T>
	constexpr inline T concat(const uint8_t* bytes, EndianType order = EndianType::BigEndian) {
		T value = 0;
		for (size_t i = 0; i < sizeof(T); ++i) {
			if (order == EndianType::BigEndian)
				value |= static_cast<T>(bytes[i]) << (8 * (sizeof(T) - 1 - i));
			else
				value |= static_cast<T>(bytes[i]) << (8 * i);
		}
		return value;
	}

	/**
	 * @brief Splits an integer value into an array of bytes.
	 *
	 * @tparam T Integer type (e.g., uint16_t, uint32_t, uint64_t).
	 * @param value The integer value to split.
	 * @param result Pointer to the array to store the bytes.
	 * @param order Byte order to use (default: BigEndian).
	 */
	template<typename T>
	constexpr inline void split(T value, uint8_t* result, EndianType order = EndianType::BigEndian) {
		for (size_t i = 0; i < sizeof(T); ++i) {
			if (order == EndianType::BigEndian)
				result[i] = static_cast<uint8_t>((value >> (8 * (sizeof(T) - 1 - i))) & 0xFF);
			else
				result[i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
		}
	}

}
