#include "control.hpp"


void SWPID::reset() {
	_previous_state = 0.0f;
	_previous_ref = 0.0f;
	_integrator = 0.0f;
	_switching_state = SystemState::StickSlip;
}

float SWPID::step(float reference, float feedback) {
	// Velocity calculation
	float velocity = (feedback - _previous_state) / _dt;

	// Tracking error
	float error = reference - feedback;

	// Proportional and Integral Action
	float sigma = _Ki * error;
	float PI = (error + _integrator) * _Kp;

	// Internal saturation (anti-windup)
	float phi = std::clamp(PI, _windup_min, _windup_max);

	// Switching logic
	if ((_switching_state == SystemState::StickSlip) && (sigma * phi <= 0.0f)) {
		phi = -phi;
		_switching_state = SystemState::Overshoot;
	}
	else if ((_switching_state == SystemState::Overshoot) && (velocity * phi >= 0.0f)) {
		phi = sigma * _Kp / _Ki;
		_switching_state = SystemState::StickSlip;
	}

	// Derivative correction
	float output = phi - _Kd * velocity;

	// Output saturation
	output = std::clamp(output, _out_min, _out_max);

	// Output dead-banding
	float filt_reference = (reference - (-0.0004 * _previous_ref)) / _dt;
	float dot_reference = 0.6 * (filt_reference - _previous_ref);

	if ((std::abs(dot_reference) < 0.01) && (std::abs(error) <= 3 * 3.1415 / 4096.0f)) {
		output = 0.0f;
		_integrator = 0.0f;
	}

	// State update
	_previous_state = feedback;
	_previous_ref = filt_reference;
	_integrator += error * _Ki * _dt;

	// Return
	return output;
}

void SWPID::changeParameter(SWPID::ControllerParam param, float new_value) {
	switch (param) {
	case ControllerParam::ProportionalGain:
		_Kp = new_value;
		break;
	case ControllerParam::IntegralGain:
		_Ki = new_value;
		break;
	case ControllerParam::DerivativeGain:
		_Kd = new_value;
		break;
	case ControllerParam::AntiwindupMax:
		_windup_max = new_value;
		break;
	case ControllerParam::AntiwindupMin:
		_windup_min = new_value;
		break;
	case ControllerParam::OutputMax:
		_out_max = new_value;
		break;
	case ControllerParam::OutputMin:
		_out_min = new_value;
		break;
	}
}


int32_t AngleUnwrapper::update(uint16_t raw_value) {
    if (_first_read) {
        _previous_raw = raw_value;
        _first_read = false;
        return static_cast<int32_t>(raw_value);
    }

    int32_t delta = static_cast<int32_t>(raw_value) - static_cast<int32_t>(_previous_raw);

    // Correggi i salti dovuti al wrap
    if (delta >  static_cast<int32_t>(_max_value) / 2) {
        // Salto indietro (da 0 a 4095)
        _turns--;
    } else if (delta < -static_cast<int32_t>(_max_value) / 2) {
        // Salto avanti (da 4095 a 0)
        _turns++;
    }

    _previous_raw = raw_value;

    // Posizione assoluta = valore attuale + giri accumulati
    int32_t absolute = static_cast<int32_t>(raw_value) + _turns * static_cast<int32_t>(_max_value);

    return absolute;
}
