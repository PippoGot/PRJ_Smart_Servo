#include "control.hpp"


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
