#pragma once

#include "Arduino.h"

#include "WiFi.h"

#include <esp_sleep.h>
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>
#include <esp_bt_main.h>

#include "driver/rtc_io.h"



// Deep Sleep https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/



class KSDeepSleep {
    public:
        KSDeepSleep() {}
        void printWakeupReason();
        bool isWakeupReasonGPIO();
        bool isWakeupReasonReset();

        void init(uint64_t time_in_us, gpio_num_t gpio_num = GPIO_NUM_MAX, int level = 1);
        void setWakeupTimer(uint64_t time_in_us);
        uint64_t getWakeupTimer() { return _lastWakeupTimer_in_us; }
        bool sleep();

        void disableBluetooth();
        void disableADC();
        void enableADC();
        void disableWiFi();
        void disableROMLogging();

        // disable deepSleep if OTA ist active
        void setOTAActive(bool active = true) { _disableBecauseOTAActive = active; }

        void setOnBeforeDeepSleepListener(void (*callback)()) { onBeforeDeepSleepListener = callback; }
    
    protected:
       void (*onBeforeDeepSleepListener)();

    private:
        uint64_t _lastWakeupTimer_in_us = 0;

        bool _disableBecauseOTAActive = false;
};




