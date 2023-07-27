#ifndef _EVENTGROUPNETWORK_H
#define _EVENTGROUPNETWORK_H




// definition of bits for EventGroupNetwork
#define EG_NETWORK_INITIALIZED (1UL << 0UL)        // zero shift for bit0  // WiFi is initialized
#define EG_NETWORK_CONNECTED (1UL << 1UL)          // 1 shift for bit1     // WiFi is connected


#define EG_NETWORK_MQTT_CONNECTING (1UL << 2UL)    // 2 shift for bit2     // MQTT connection active
#define EG_NETWORK_MQTT_CONNECTED (1UL << 3UL)     // 3 shift for bit3     // MQTT is connected

#define EG_NETWORK_NTP_SYNCING  (1UL << 4UL)       // 4 shift for bit4     // NTP sync active
#define EG_NETWORK_NTP_SYNCED  (1UL << 5UL)        // 5 shift for bit5     // NTP is synced

#define EG_NETWORK_FS_AVAILABLE (1UL << 6UL)       // 6 shift for bit6     // FileSystem is available
#define EG_NETWORK_SAVE_ACTIVE (1UL << 7UL)        // 7 shift for bit7     // Save of data is active

#define EG_NETWORK_JET32_CONNECTING (1UL << 8UL)       // 8 shift for bit8     // Jet32 connecting active
#define EG_NETWORK_JET32_CONNECTED (1UL << 9UL)        // 9 shift for bit9     // Jet32 connected


/*
    // set Events
    if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
        if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_DISPLAY_INITIALIZED) == 0) {
            xEventGroupSetBits(*_phEventGroupNetwork, EG_DISPLAY_INITIALIZED);
//            Serial.println(F("[display] Set Event EG_DISPLAY_INITIALIZED"));
        }
    }

    
    // Wait for Evemts
    if (_hEventGroup) {
		// prüfen, ob Zeichnen zur Verfügung steht, ansonsten warten, bis frei
        if ((xEventGroupGetBits(_hEventGroup) & KSCRITICALSECTION_FREE) == 0) {
            //Serial.println(F("[KSCriticalSection] Wating for Event KSCRITICALSECTION_FREE"));
            EventBits_t eventGroupValue;
            eventGroupValue = xEventGroupWaitBits(_hEventGroup, KSCRITICALSECTION_FREE, pdFALSE, pdTRUE, portMAX_DELAY);
            //Serial.println(F("[KSCriticalSection] Event KSCRITICALSECTION_FREE set"));
        }
    }

    // Reset Events
    if ((xEventGroupGetBits(_hEventGroup) & KSCRITICALSECTION_FREE) == KSCRITICALSECTION_FREE) {
        xEventGroupClearBits(_hEventGroup, KSCRITICALSECTION_FREE);
        //Serial.println(F("[KSCriticalSection] Enter Critical Section"));
    }
*/

#endif  // #define _EVENTGROUPNETWORK_H