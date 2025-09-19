#include "control.hpp"


// --- Controller constructor -----------------------------------------------------------

PIDController::PIDController(float dt, float Kp, float Ki, float Kd) :
		_dt(dt), _Kp(Kp), _Ki(Ki), _Kd(Kd) {}


// --- Controller core method -----------------------------------------------------------

float PIDController::compute(float e_k) {
	// Proportional
	float P = _Kp * e_k;

	// Integral (cumulative and anti-windup)
	float I_candidate = _integral + (_Ki * _dt * (e_k + _e_k1)) / 2.0f;

	// Derivative (discrete derivative filter)
    float D = _Kd * (e_k - _e_k1) / _dt;

    //_antiWindupClamp(e_k, P, D);
	//float I = _integral;

    // Candidate output
    float u_raw = P + I_candidate + D;

    // Saturation
    float u_sat = _clamp(u_raw, _out_min, _out_max);

    // Clamping logic
    bool update_integrator = true;
    if (u_raw != u_sat) {
    	if ((u_sat >= _out_max && e_k > 0) || (u_sat <= _out_min && e_k < 0)) {
    		update_integrator = false;
    	}
    }

    if (update_integrator) {
    	_integral = I_candidate;
    }

    // Update error and return
    _e_k1 = e_k;

    return u_sat;
}

void PIDController::reset(void) {
	_integral = 0.0f;
	_e_k1 = 0.0f;
}

void PIDController::setOutputLimits(float min, float max) {
	_out_min = min;
	_out_max = max;
}

// --- Controller helper methods --------------------------------------------------------

float PIDController::_clamp(float x, float lo, float hi) {
	if (x < lo) return lo;
	if (x > hi) return hi;
	return x;
}

float PIDController::_limitOutput(float input) {
	if (input > _out_max) return _out_max;
	if (input < _out_min) return _out_min;
	return input;
}


// --------------------------------------------- SWPIDController class implementation ---

// --- Controller constructor -----------------------------------------------------------

SWPIDController::SWPIDController(float dt, float Kp, float Ki, float Kd) :
		PIDController(dt, Kp, Ki, 0.0f) {}

// --- Controller core method (partial override) ----------------------------------------

float SWPIDController::compute(float x_ref, float x_k) {
	// Pre operations
	float velocity = (x_k - _x_k1) / _dt;
	float e_k = x_ref - x_k;

	float sigma = _Ki * e_k;

	// Superclass method call
	float phi = PIDController::compute(e_k);

	// Post operations
	if (SYSTEM_STATE::STICK_SLIP == _st && 0 >= sigma*phi) {
		// Transition to OVERSHOOT
		phi = -phi;
		_st = SYSTEM_STATE::OVERSHOOT;
	}
	else if (SYSTEM_STATE::OVERSHOOT && 0 <= velocity*phi) {
		// Transition to STICK_SLIP
		phi = sigma * _Kp / _Ki;
		_st = SYSTEM_STATE::STICK_SLIP;
	}

	return phi - _Kd * velocity;
}
