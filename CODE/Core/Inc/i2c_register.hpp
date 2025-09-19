/*
 * i2c_register.hpp
 *
 * Module: Virtual I2C Registers
 * Description:
 *  This header defines two classes to create a virtual I2C register map.
 *  - VirtualRegister<T>: Represents a single register of type T (uint8_t, uint16_t, float, etc.).
 *    It stores the value internally as a byte array for easy I2C transmission and provides
 *    methods to read/write both in raw bytes and typed values.
 *  - RegisterMap: Maintains a mapping between register addresses and their corresponding
 *    VirtualRegister instances, allowing easy access by address.
 *
 *  Usage:
 *    1. Create VirtualRegister instances with specific types and addresses.
 *    2. Append them to a RegisterMap instance.
 *    3. Access registers by address and use get/set or read/write methods.
 *
 *  Author: Pippogot
 *  Created on: Sep 11, 2025
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>



namespace I2C {

	#define MAX_REGISTERS 256


	class IVirtualRegister {
	public:
		IVirtualRegister(uint8_t addr, uint8_t size) : _address(addr), _size(size) {
			content = new uint8_t[_size];
			std::memset(content, 0, _size);
		}

		virtual ~IVirtualRegister() = default;


	    const uint8_t size() const { return _size; }
	    const uint8_t address() const { return _address; }


		uint8_t* content;

	protected:
		const uint8_t _address;
		const uint8_t _size;
	};


    struct RegisterEntry {
    	uint8_t address;
    	IVirtualRegister* reg;
    };


    class RegisterMap {
    public:
    	RegisterMap() : _count(0) {}

    	bool append(IVirtualRegister& reg) {
    		if (_count >= MAX_REGISTERS) return false;

    		_entries[_count].address = reg.address();
    		_entries[_count].reg = &reg;
    		_count++;
    		return true;
    	}

    	IVirtualRegister* access(uint8_t addr) {
    		for (uint8_t i = 0; i < _count; i++) {
    			if (_entries[i].address == addr) return _entries[i].reg;
    		}

    		return nullptr;
    	}

    	uint8_t size() const { return _count; }


    protected:
    	RegisterEntry _entries[MAX_REGISTERS];
    	uint8_t _count;
    };


    template<typename T>
    class VirtualRegister : public IVirtualRegister{
    public:

    	// --- Constructor
    	VirtualRegister(uint8_t addr) : IVirtualRegister(addr, sizeof(T)) {}


    	VirtualRegister(uint8_t addr, RegisterMap& map) : IVirtualRegister(addr, sizeof(T)) {
    		map.append(*this);
    	}


    	T typedGet() const {
    		T value{};
    		std::memcpy(&value, content, _size);
    		return value;
    	}
    	void typedSet(T value) { std::memcpy(content, &value, _size); }
    };
}
