/*
 * control.hpp
 *
 *  Module containing classes and function for system control.
 *
 */

#pragma once

#include <algorithm>
#include <cstdint>


// -------------------------------------------------- PIDController class declaration ---

class PIDController {

public:

	// --- Controller constructor -------------------------------------------------------

	PIDController(float dt, float Kp, float Ki, float Kd);

	// --- Filter core method -----------------------------------------------------------

	float compute(float e_k);
	void reset(void);
	void setOutputLimits(float min, float max);

protected:

	// --- Helper methods ---------------------------------------------------------------

	float _clamp(float x, float lo, float hi);
	float _limitOutput(float input);

	// --- Controller variables ---------------------------------------------------------

	// Time step
	float _dt;

	// Gains
	float _Kp, _Ki, _Kd;

	// State variables
	float _integral = 0.0f;
	float _e_k1 = 0.0f;

	// Output saturation
	float _out_min = -1e6f;
	float _out_max = 1e6f;

};


// ------------------------------------------------ SWPIDController class declaration ---

class SWPIDController : public PIDController {

public:

	// --- Controller constructor -------------------------------------------------------

	SWPIDController(float dt, float Kp, float Ki, float Kd);

	// --- Controller core method -------------------------------------------------------

	float compute(float x_ref, float x_k);
	float compute(uint16_t x_ref, uint16_t x_k, float convert) { return compute(convert * x_ref, convert * x_k); }

protected:

	// --- Controller states ------------------------------------------------------------

	enum SYSTEM_STATE : int {
		STICK_SLIP = 0,
		OVERSHOOT = 1,
	};

	// --- Controller variables ---------------------------------------------------------

	float _x_k1 = 0.0f;
	SYSTEM_STATE _st = STICK_SLIP;

};
