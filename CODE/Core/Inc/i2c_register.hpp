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
#include <functional>


namespace I2C {

	#define MAX_REGISTERS 256

	enum class RegisterMode : uint8_t {
		ReadWrite,
		ReadOnly
	};


	class IVirtualRegister {
	public:
		IVirtualRegister(uint8_t addr, uint8_t size, RegisterMode mode = RegisterMode::ReadWrite)
		  :	_address(addr), _size(size), _mode(mode), _content(new uint8_t[_size]) {
			std::memset(_content, 0, _size);
		}

		virtual ~IVirtualRegister() {
			delete[] _content;
		}


	    const uint8_t size() const { return _size; }
	    const uint8_t address() const { return _address; }
	    const RegisterMode mode() const { return _mode; }

	    uint8_t* rawRead() { return _content; }

	    virtual bool rawWrite(const uint8_t* data, size_t len) {
	    	if (_mode == RegisterMode::ReadOnly) return false;
	    	if (len > _size) return false;

	    	std::memcpy(_content, data, len);

	    	if (_onChange) _onChange();
	    	return true;
	    }

	    void bind(std::function<void()> cb) {
	    	_onChange = cb;
	    }


	protected:
		const uint8_t _address;
		const uint8_t _size;
		const RegisterMode _mode;

		uint8_t* _content;

		std::function<void()> _onChange;
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
    	VirtualRegister(uint8_t addr, RegisterMode mode = RegisterMode::ReadWrite)
    		: IVirtualRegister(addr, sizeof(T), mode) {}


    	VirtualRegister(uint8_t addr, RegisterMap& map, RegisterMode mode = RegisterMode::ReadWrite)
    		: IVirtualRegister(addr, sizeof(T), mode) {
    		map.append(*this);
    	}


    	T typedGet() const {
    		T value{};
    		std::memcpy(&value, _content, _size);
    		return value;
    	}

    	void typedSet(T value) {
    		std::memcpy(_content, &value, _size);

    		if (this -> _onChange) this -> _onChange();
    	}
    };
}
