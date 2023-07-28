/**
 * @file KSNTPClient.h
 * 
 * @brief Header file for KSNTPClient class
 * 
 * @details 
 * 
 * @see
 *   - paulstoffregen/Time @ ^1.6.1
 * 
 * @author Joachim Kittelberger <jkittelberger@kibesoft.de>
 * @date 20.06.2022
 * @version 1.00
 *
 * @todo
 *
 * @bug
 */

/**
 * @copyright
 * Copyright (C) 2022, KibeSoft - Joachim Kittelberger, (https://www.kibesoft.de)
 * All rights reserved
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     https://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#ifndef _KSNTPCLIENT_H
#define _KSNTPCLIENT_H


/*
#ifdef ARDUINO_ARCH_ESP32
    #include <WiFi.h>  // we need wifi to get internet access
#endif
#ifdef ARDUINO_ARCH_ESP8266
    #include <ESP8266WiFi.h> 
#endif
*/

#if defined(ESP8266)
    //#pragma message "ESP8266 is defined"
    #include <ESP8266WiFi.h>
#elif defined(ESP32)
    //#pragma message "ESP32 is defined"
    #include <WiFi.h>
#else
#error "This isn't a ESP8266 or ESP32!"
#endif


#include "KSRTC.h"
#include "KSUtilities.h"


#define NTP_SYNC_INTERVAL_IN_MIN 6*60            // synchronization-interval
//#define NTP_SYNC_INTERVAL_IN_MIN 2            // synchronization-interval for testing

#define TIMEZONE_GERMANY "CET-1CEST,M3.5.0,M10.5.0/3"           // Timezone for Berlin

class KSNTPClient {
    public:
        KSNTPClient();
        ~KSNTPClient();

        TaskHandle_t createConnection(EventGroupHandle_t *phEventGroupNetwork = NULL, KSRTC* pRTC = NULL);
        void waitForSync();
        bool isInSync() { return _bIsInSync; }

    private:
        void tKSNTPClient();

        bool isAdjusted() { return _bTimeAdjusted; }

        bool _bTimeAdjusted = false;        // has minimum one synchronization
        bool _bIsInSync = false;            // is in Sync in the needed syncinterval NTP_SYNC_INTERVAL_IN_MIN
        KSRTC* _pRTC = NULL;                // Hardware RTC or SW-Lib

        TaskHandle_t _htKSNTPClient;
        EventGroupHandle_t *_phEventGroupNetwork = NULL;
};




#endif  // #define _KSNTPCLIENT_H