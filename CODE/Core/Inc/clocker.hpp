/**
 * @file clocker.hpp
 * @brief Execution time measurement utility for STM32F103C8T6.
 *
 * Features:
 *  - Uses DWT cycle counter (CYCCNT).
 *  - Configurable CPU clock frequency (default: SystemCoreClock).
 *  - Supports multiple snapshots within a single measurement cycle.
 *
 * @author	Pippogot
 * @date 	23/08/2025
 */

#pragma once

#include "stm32f1xx.h"
#include <array>


class Clocker {

public:

	// --- Constructor

    Clocker(uint32_t _cpu_freq_hz = SystemCoreClock) : _cpu_freq(_cpu_freq_hz) {
        initDWT();
    }

    // --- Core Methods

    /// Change CPU frequency dynamically (Hz)
    inline void setCpuFrequency(uint32_t hz) {
        _cpu_freq = hz;
    }

    /// Start measurement cycle
    inline void start() {
        // Reset cycle counter
        DWT -> CYCCNT = 0;
        _start_val = DWT -> CYCCNT;
        snapshot_index = 0;
    }

    /// Stop measurement cycle
    inline void stop() {
        _stop_val = DWT -> CYCCNT;
    }

    /// Take a snapshot (intermediate measurement)
    inline void snapshot() {
        if (snapshot_index < MAX_SNAPSHOTS) {
            snapshots[snapshot_index++] = DWT -> CYCCNT;
        }
    }

    /// Get total cycles between start() and stop()
    inline uint32_t cycles() const {
        return _stop_val - _start_val;
    }

    /// Get total elapsed time in microseconds
    inline float micros() const {
        return static_cast<float>(cycles()) / (_cpu_freq / 1'000'000.0f);
    }

    /// Get total elapsed time in milliseconds
    inline float millis() const {
        return static_cast<float>(cycles()) / (_cpu_freq / 1'000.0f);
    }

    /// Get cycles between two snapshots
    inline uint32_t cyclesBetween(size_t snap1, size_t snap2) const {
        if (snap1 < snapshot_index && snap2 < snapshot_index && snap2 > snap1) {
            return snapshots[snap2] - snapshots[snap1];
        }
        return 0;
    }

    /// Get elapsed time in µs between two snapshots
    inline float microsBetween(size_t snap1, size_t snap2) const {
        return static_cast<float>(cyclesBetween(snap1, snap2)) / (_cpu_freq / 1'000'000.0f);
    }

    /// Get elapsed time in ms between two snapshots
    inline float millisBetween(size_t snap1, size_t snap2) const {
        return static_cast<float>(cyclesBetween(snap1, snap2)) / (_cpu_freq / 1'000.0f);
    }

private:

    // --- Variables

    uint32_t _cpu_freq;

    uint32_t _start_val = 0;
    uint32_t _stop_val  = 0;

    static constexpr size_t MAX_SNAPSHOTS = 8;
    std::array<uint32_t, MAX_SNAPSHOTS> snapshots{};

    size_t snapshot_index = 0;

    // --- Init Method

    void initDWT() {
        if (!(CoreDebug -> DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
            CoreDebug -> DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        }

        DWT -> CYCCNT = 0;
        DWT -> CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
};
