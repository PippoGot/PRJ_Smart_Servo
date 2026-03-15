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


namespace mth {
	constexpr float PI = 3.14159265358979323846f;

	template<class T>
    constexpr T clamp(const T& v, const T& lo, const T& hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

	// Symmetric overload
    template<class T>
    constexpr T clamp(const T& v, const T& th) {
        return clamp(v, -th, th);
    }


	template<class T>
	constexpr T deadzone(const T& v, const T& lo, const T& hi) {
		return v - clamp(v, lo, hi);
	}

	// Symmetric overload
    template<class T>
    constexpr T deadzone(const T& v, const T& th) {
        return deadzone(v, -th, th);
    }


	template<class T>
	constexpr int sign(const T& v) {
		return (T(0) < v) - (v < T(0));
	}


    template<class T>
    constexpr T abs(T v) {
        return (v < T(0)) ? -v : v;
    }
}


class DiscreteBlock {
public:
	virtual ~DiscreteBlock() = default;

	virtual float step(float input) = 0;
	virtual void reset(float value = 0.0f) = 0;
};


class DiscreteDelay : public DiscreteBlock {
public:

	DiscreteDelay() : _prev_input(0.0f) {}

	float step(float input) override {
		float output = _prev_input;
		_prev_input = input;
		return output;
	}

	float state() { return _prev_input; }

	void reset(float value = 0.0f) override { _prev_input = value; }


protected:
	float _prev_input;
};


class DiscreteIntegrator : public DiscreteBlock {
public:

	DiscreteIntegrator(float Ts, float min_out = -1e6f, float max_out = 1e6f, float K = 1.0f)
		: _Ts(Ts), _min_out(min_out), _max_out(max_out), _K(K),
		  _delay(), _first_step(true), _first_value(0.0f) {}

	float step(float input) override {
		if (_first_step) {
			_delay.reset(_first_value);
			_first_step = false;
		}

		float prev = _delay.state();

		float state = prev + _K * _Ts * input;
		state = mth::clamp(state, _min_out, _max_out);

		_delay.step(state);

		return state;
	}

	float state() { return mth::clamp(_delay.state(), _min_out, _max_out); }

	void reset(float value = 0.0f) override { _first_step = true; _first_value = value; }


protected:
	float _Ts;
	float _min_out, _max_out;
	float _K;
	DiscreteDelay _delay;
	bool _first_step;
	float _first_value;
};


class DiscreteDerivative : public DiscreteBlock {
public:

	DiscreteDerivative(float Ts, float K = 1.0f)
		: _Ts(Ts), _K(K), _delay(), _first_step(true) {}

	float step(float input) override {
		if (_first_step) {
			_delay.reset(input);
			_first_step = false;

			// No derivative for first step
			return 0.0f;
		}

		float prev = _delay.step(input);

		float output = _K * (input - prev) / _Ts;
		return output;
	}

	void reset(float value = 0.0f) override { _first_step = true; }


protected:
	float _Ts, _K;
	DiscreteDelay _delay;
	bool _first_step;
};


class GlitchFilter : public DiscreteBlock {
public:

	GlitchFilter(float max_delta)
		: _max_delta(max_delta), _delay(), _first_step(true) {}

	float step(float input) override {
		// First Step: initialize the delay
		if(_first_step){
			_delay.reset(input);
			_first_step = false;
			return input;
		}

		// Get previous value from delay
		float prev = _delay.step(input);
		float delta = input - prev;

		float output = input;

		// If a glitch is detected use previous value
		if (std::fabs(delta) >= _max_delta) {
			output = prev;
		}

		return output;
	}

	void reset(float value = 0.0f) override { _first_step = true; }


protected:
	float _max_delta;
	DiscreteDelay _delay;
	bool _first_step;
};


class DiscreteFilter : public DiscreteBlock {
public:

	DiscreteFilter(float a1, float a0, float b1, float b0)
		: _a1(a1), _a0(a0), _b1(b1), _b0(b0),
		  _input_delay(), _output_delay(), _first_step(true) {}

	float step(float input) override {
		if (_first_step) {
			_input_delay.reset(0.0f);
			_output_delay.reset(0.0f);
			_first_step = false;
		}

		// Previous input and output
		float in_prev = _input_delay.step(input);
		float out_prev = _output_delay.state();

		// New output
		float output = (_a1 * in_prev + _a0 * input - _b1 * out_prev) / _b0;
		_output_delay.step(output);

		return output;
	}

	float state() { return _output_delay.state(); }

	void reset(float value = 0.0f) override {
		_first_step = true;
	}


protected:
	float _a1, _a0;
	float _b1, _b0;

	DiscreteDelay _input_delay, _output_delay;

	bool _first_step;
};


class SwitchingPID {
public:

	SwitchingPID(float Ts, float Kp, float Ki, float Kd)
		: _Ts(Ts), _Kp(Kp), _Ki(Ki), _Kd(Kd),
		  _windup_min(-1e6f), _windup_max(1e6f),
		  _out_min(-1e6f), _out_max(1e6f),
		  _integrator(_Ts), _derivative(_Ts), _glitch_filter(100.0f),
		  _ref_filter(0.6f, -0.6f, 1e-3f, -4e-3f),
		  _switching_state(SystemState::StickSlip) {}

	float step(float reference, float feedback) {
		// Error calculation
		float error = reference - feedback;

		// Sigma and Phi
		float sigma = _Ki * error;
		float raw_phi = _Kp * (error + _integrator.state());
		float phi = mth::clamp(raw_phi, _windup_min, _windup_max);

		// Anti-windup
		bool cond_1 = mth::sign(sigma) == mth::sign(mth::deadzone(raw_phi, _windup_min, _windup_max));
		bool cond_2 = mth::deadzone(raw_phi, _windup_min, _windup_max) != 0;

		if (cond_1 && cond_2) {
			_integrator.step(0);
		}
		else {
			_integrator.step(sigma);
		}

		// Velocity
		float velocity = _glitch_filter.step(_derivative.step(feedback));

		// Switching logic
		if ((_switching_state == SystemState::StickSlip) && (sigma * phi <= 0)) {
			// Transition to Overshoot
			phi = -phi;
			_integrator.reset((phi / _Kp) - error);
			_switching_state = SystemState::Overshoot;
		}
		else if ((_switching_state == SystemState::Overshoot) && (velocity * phi >= 0)) {
			// Transition to StickSlip
			phi = _Kp * sigma / _Ki;
			_integrator.reset();
			_switching_state = SystemState::StickSlip;
		}
		// Otherwise keep previous state

		// Output calculation
		float command = mth::clamp(phi - _Kd * velocity, _out_min, _out_max);

		// Dead-banding if needed
		/*
		float ref_velocity = mth::clamp(mth::abs(_ref_filter.step(reference)), 1.0f);

		float th_velocity = 0.01; 			// param 1
		float LSB = 2 * mth::PI / 4096.0f;	// param 2
		uint8_t steps = 5;					// param 3

		if ((ref_velocity < th_velocity) && (mth::abs(error) < LSB * steps)) {
			command = 0.0f;
		}
		*/
		// Return command
		return command;
	}


	enum class ControllerParam : uint8_t {
		ProportionalGain,
		IntegralGain,
		DerivativeGain,

		AntiwindupMax,
		AntiwindupMin,

		OutputMax,
		OutputMin,
	};

	void changeParameter(ControllerParam param, float new_value) {
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


protected:
	float _Ts;
	float _Kp, _Ki, _Kd;

	float _windup_min, _windup_max;
	float _out_min, _out_max;

	DiscreteIntegrator _integrator;
	DiscreteDerivative _derivative;
	GlitchFilter _glitch_filter;
	DiscreteFilter _ref_filter;

	// Switching state variables
	enum class SystemState : int8_t {
		StickSlip = -1,
		Overshoot = 1,
	};

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

