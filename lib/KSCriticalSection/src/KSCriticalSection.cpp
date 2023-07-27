
#include "KSCriticalSection.h"



KSCriticalSection::KSCriticalSection() {
    _hEventGroup = xEventGroupCreate();
    assert(_hEventGroup);
    LeaveCriticalSection();     // init Critcal Section as free
}


KSCriticalSection::~KSCriticalSection() {
}



void KSCriticalSection::EnterCriticalSection() {
	// Warte auf Freie Critical Section:
    if (_hEventGroup) {
		// prüfen, ob Zeichnen zur Verfügung steht, ansonsten warten, bis frei
        if ((xEventGroupGetBits(_hEventGroup) & KSCRITICALSECTION_FREE) == 0) {
            //Serial.println(F("[KSCriticalSection] Wating for Event KSCRITICALSECTION_FREE"));
            EventBits_t eventGroupValue;
            eventGroupValue = xEventGroupWaitBits(_hEventGroup, KSCRITICALSECTION_FREE, pdFALSE, pdTRUE, portMAX_DELAY);
//            eventGroupValue = xEventGroupWaitBits(_hEventGroup, KSCRITICALSECTION_FREE, pdFALSE, pdTRUE, pdMS_TO_TICKS(2000));
            // Check for timeout
            if ((eventGroupValue & KSCRITICALSECTION_FREE) == 0) {
                Serial.print(F("[KSCriticalSection] Event KSCRITICALSECTION_FREE timedout (2s) in task: "));
                Serial.println(String(pcTaskGetTaskName(NULL)).c_str());
            }
            //Serial.println(F("[KSCriticalSection] Event KSCRITICALSECTION_FREE set"));
        }

		// Enter Event
        if ((xEventGroupGetBits(_hEventGroup) & KSCRITICALSECTION_FREE) != 0) {
            xEventGroupClearBits(_hEventGroup, KSCRITICALSECTION_FREE);
            //Serial.println(F("[KSCriticalSection] Enter Critical Section"));
        }
    }
}




void KSCriticalSection::LeaveCriticalSection() {
	// Critical Section freigeben:
    if (_hEventGroup) {
        if ((xEventGroupGetBits(_hEventGroup) & KSCRITICALSECTION_FREE) == 0) {
            xEventGroupSetBits(_hEventGroup, KSCRITICALSECTION_FREE);
            //Serial.println(F("[KSCriticalSection] Release Critical Section"));
        }
    }
}

   


