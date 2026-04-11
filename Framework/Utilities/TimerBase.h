/*
 * TimerBase.h
 *
 *  Created on: Apr 11, 2026
 *      Author: dirktjantele
 */

#pragma once

#include "FreeRTOS.h"
#include "timers.h"

namespace CasualNoises
{

class TimerBase
{
public:
    TimerBase(const char* name, uint32_t periodMs, bool autoReload = true)
        : _period(periodMs)
    {
        _timer = xTimerCreate(
            name,
            pdMS_TO_TICKS(periodMs),
            autoReload ? pdTRUE : pdFALSE,
            this,                 // ← store "this" as timer ID
            &TimerBase::timerCallbackStatic
        );
    }

    virtual ~TimerBase() {
        if (_timer) {
            xTimerDelete(_timer, 0);
        }
    }

    void start() {
        xTimerStart(_timer, 0);
    }

    void stop() {
        xTimerStop(_timer, 0);
    }

protected:
    virtual void onTimer() = 0;   // ← derived classes override this

private:
    static void timerCallbackStatic(TimerHandle_t xTimer) {
        // Recover the object pointer
        TimerBase* obj = static_cast<TimerBase*>(pvTimerGetTimerID(xTimer));
        if (obj) {
            obj->onTimer();       // Call the virtual method
        }
    }

    TimerHandle_t _timer;
    uint32_t _period;
};

} // namespace CasualNoises
