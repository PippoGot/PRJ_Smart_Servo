/*
 * control.hpp
 *
 *  Module containing classes and function for system control.
 *
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <cmath>


class SWPID {
public:

	SWPID(float dt, float Kp, float Ki, float Kd)
		: _dt(dt), _Kp(Kp), _Ki(Ki), _Kd(Kd),
		  _windup_max(1e6f), _windup_min(-1e6f), _out_max(1e6f), _out_min(-1e6f)
	{
		reset();
	}


	void reset();


	float step(float reference, float feedback);


	enum class ControllerParam : uint8_t {
		ProportionalGain,
		IntegralGain,
		DerivativeGain,

		AntiwindupMax,
		AntiwindupMin,

		OutputMax,
		OutputMin,
	};

	void changeParameter(ControllerParam param, float new_value);


protected:

	// Controller parameters
	float _dt;

	float _Kp;
	float _Ki;
	float _Kd;

	float _windup_max;
	float _windup_min;

	float _out_max;
	float _out_min;


	// State variables
	enum class SystemState : int8_t {
		StickSlip = -1,
		Overshoot = 1,
	};

	float _previous_state;
	float _previous_ref;
	float _integrator;
	SystemState _switching_state;
};



class AngleUnwrapper {
public:
    AngleUnwrapper(uint16_t max_value = 4096)
        : _max_value(max_value), _previous_raw(0), _turns(0), _first_read(true) {}


    int32_t update(uint16_t raw_value);


private:
    int _max_value;
    int _previous_raw;
    long _turns;
    bool _first_read;
};

