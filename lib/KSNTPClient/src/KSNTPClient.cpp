#include <TimeLib.h>        // evtl nicht vorhanden


#include "KSNTPClient.h"
#include "KSEventGroupNetwork.h"


//const char* ntpServer = "pool.ntp.org";
const char* ntpServer = "de.pool.ntp.org";

const long gmtOffset_sec = 1*60*60;		// UTC + 1.00
const int daylightOffset_sec = 3600;	// Sommerzeit 3600 ansonsten 0



KSNTPClient::KSNTPClient() {
}


KSNTPClient::~KSNTPClient() {
}


TaskHandle_t KSNTPClient::createConnection(EventGroupHandle_t *phEventGroupNetwork, KSRTC* pRTC) {

	_phEventGroupNetwork = phEventGroupNetwork;
    _pRTC = pRTC;

	int coreID = xPortGetCoreID();
	//Serial.print(F("CoreID: "));
	//Serial.println(coreID);
	
	UBaseType_t setupPriority = uxTaskPriorityGet(NULL);
	//Serial.print(F("setup: priority = "));
	//Serial.println(setupPriority);

	xTaskCreatePinnedToCore(
    	[](void* context){ static_cast<KSNTPClient*>(context)->tKSNTPClient(); },
		"tKSNTPClient",
		8192,
		this,
		setupPriority,
		&_htKSNTPClient,
		coreID
		);

	return _htKSNTPClient;
}



void KSNTPClient::tKSNTPClient()
{
    int countMinutesForResync = 0;

	// main loop ntp
    for (;;) {

        // alle Stunde mal aktualisieren, sofern Wifi-Verbindung da Initialisierung

        // Wenn !_bIsInSync dann alle Minute versuchen. Ansonsten eine Stunde warten
        // nochmals unterscheiden, ob schon eine Verbindung da war und nur das ReSync nicht funktioniert hat.
        if (_bIsInSync) {
            if (countMinutesForResync < NTP_SYNC_INTERVAL_IN_MIN) {
                countMinutesForResync++;
                vTaskDelay(pdMS_TO_TICKS(1000*60*1));       // alle minute aktualisieren
                continue;
            } else {
                countMinutesForResync = 0;
                _bIsInSync = false;
                
                // Reset EG_NETWORK_NTP_SYNCED
                if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
                    if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_NTP_SYNCED) == EG_NETWORK_NTP_SYNCED) {
                        xEventGroupClearBits(*_phEventGroupNetwork, EG_NETWORK_NTP_SYNCED);
                    }
                }
            }
        }


        // Warte auf Wifi-Verbindung und Initialisierung:
        // Wenn Connection-Bit noch nicht gesetzt, dann das erste Mal warten.
        if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
            if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
                //Serial.println(F("[ntp] Wating for Event EG_NETWORK_CONNECTED"));

                // Reset EG_NETWORK_NTP_SYNCING
                if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_NTP_SYNCING) == EG_NETWORK_NTP_SYNCING) {
                    xEventGroupClearBits(*_phEventGroupNetwork, EG_NETWORK_NTP_SYNCING);
                }

                EventBits_t eventGroupValue;
                eventGroupValue = xEventGroupWaitBits(*_phEventGroupNetwork, (EG_NETWORK_INITIALIZED | EG_NETWORK_CONNECTED), pdFALSE, pdTRUE, portMAX_DELAY);
                //Serial.println(F("[ntp] Event EG_NETWORK_CONNECTED set"));
            }

            // set EG_NETWORK_NTP_SYNCING
            if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_NTP_SYNCING) == 0) {
                xEventGroupSetBits(*_phEventGroupNetwork, EG_NETWORK_NTP_SYNCING);
            }
        }


        // hier muss eine WiFi-Verbindung da sein.
        Serial.print(F("[ntp] Synchronizing with NTP-Server: "));
        Serial.print(ntpServer);
        Serial.println(F(" ..."));
//	    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);       // init and read from ntp server

#if defined(ESP32)
	    configTime(0, 0, ntpServer);       // init and read from ntp server
  		//vTaskDelay(pdMS_TO_TICKS(100));       // warte, bis neue Zeit geschrieben ist. Ansonsten hat getLocalTime eine falsche Zeit

#elif defined(ESP8266)
	    configTime(TIMEZONE_GERMANY, ntpServer);       // init and read from ntp server
#else
#error "This isn't a ESP8266 or ESP32!"
#endif

        struct tm timeinfo = { 0 };
//        if (::getLocalTime(&timeinfo)) {
        if (getLocalTime(&timeinfo, 7000)) {                // TODO: hier mit Timeout arbeiten z.B. 3sec
            // erst hier Zeitzone einstellen
 
            // set the timezone for internal use in localtime_r() or localtime() or getLocalTime()
            // utc dann mit gmtime_r() ermitteln
            setenv("TZ", TIMEZONE_GERMANY, 1);
            tzset();
            
            // Set timezone to China Standard Time
            //setenv("TZ", "CST-8CDT-8,M4.2.0/2,M9.2.0/3", 1);
            //tzset();
        
#

            _bTimeAdjusted = true;
            _bIsInSync = true;

            // if we have a RTC-Lib. Set the RTC time
            struct tm timeinfolocal = { 0 };
            getLocalTime(&timeinfolocal);
            if (_pRTC) {
                _pRTC->adjust(&timeinfolocal);
            }
        	Serial.print(&timeinfolocal, "[ntp] synchronized: %A, %d. %B %Y %H:%M:%S");
            Serial.println(timeinfolocal.tm_isdst ? " Sommerzeit" : " Winterzeit (Std)");

            // Reset EG_NETWORK_NTP_SYNCING
            // set EG_NETWORK_NTP_SYNCED
			if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
				if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_NTP_SYNCING) == EG_NETWORK_NTP_SYNCING) {
					xEventGroupClearBits(*_phEventGroupNetwork, EG_NETWORK_NTP_SYNCING);
				}
				if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_NTP_SYNCED) == 0) {
					xEventGroupSetBits(*_phEventGroupNetwork, EG_NETWORK_NTP_SYNCED);
				}
			}
        } else {
            _bIsInSync = false;
            Serial.println("[ntp] Failed to obtain time from NTP-Server");
 
            // Reset EG_NETWORK_NTP_SYNCED
            if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
                if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_NTP_SYNCED) == EG_NETWORK_NTP_SYNCED) {
                    xEventGroupClearBits(*_phEventGroupNetwork, EG_NETWORK_NTP_SYNCED);
                }
            }
        }

		vTaskDelay(pdMS_TO_TICKS(1000*10));       // alle 10s aktualisieren
	}
}



void KSNTPClient::waitForSync() {
    if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
        Serial.println(F("[ntp] Waiting for Event EG_NETWORK_NTP_SYNCED"));
        EventBits_t eventGroupValue;
        eventGroupValue = xEventGroupWaitBits(*_phEventGroupNetwork, EG_NETWORK_NTP_SYNCED, pdFALSE, pdTRUE, portMAX_DELAY);
        Serial.println(F("[ntp] Event EG_NETWORK_NTP_SYNCED set"));
    }
}

