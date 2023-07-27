#include "KSWiFiConnection.h"
#include "KSEventGroupNetwork.h"
#include "KSCredentials.h"            // include global credential file



//const char* ssid = WIFI_SSID;
//const char* password = WIFI_PASSWORD;
extern const char wifi_ssid[];
extern const char wifi_password[];

#define WIFI_CHECK_CONNECTION_STATE_TIME_MS 3000  // 10 second to check for connection status
#define WIFI_TIMEOUT_MS 20000 // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 10000 // Wait 30 seconds after a failed connection attempt



KSWiFiConnection::KSWiFiConnection(uint8_t pinLEDWifiStatus) {

	WiFi.persistent(false);   // auskommentieren wenn Netzwerkname oder Passwort in den Flash geschrieben werden sollen

    // init WiFi
    WiFi.mode(WIFI_STA);
	WiFi.disconnect();
    //WiFi.setAutoReconnect(true);

    _pinLEDWifiStatus = pinLEDWifiStatus;

    // if we have a status LED -> switch off
    if (_pinLEDWifiStatus) {
        pinMode(_pinLEDWifiStatus, OUTPUT);
        digitalWrite(_pinLEDWifiStatus, LOW);
   }
}


KSWiFiConnection::~KSWiFiConnection() {
}


TaskHandle_t KSWiFiConnection::createConnection(EventGroupHandle_t *phEventGroupNetwork) {

    _phEventGroupNetwork = phEventGroupNetwork;

    int coreID = xPortGetCoreID();
    //Serial.print("CoreID: ");
    //Serial.println(coreID);
    
    UBaseType_t setupPriority = uxTaskPriorityGet(NULL);
    //Serial.print("setup: priority = ");
    //Serial.println(setupPriority);

    xTaskCreatePinnedToCore(
        [](void* context){ static_cast<KSWiFiConnection*>(context)->tKSWiFiConnection(); },
        "tKSWiFiConnection",
        8192,
        this,
        setupPriority,
        &_htKSWiFiConnection,
        coreID
        );

    return _htKSWiFiConnection;
}



void KSWiFiConnection::tKSWiFiConnection()
{
//	WiFi.onEvent(WiFiEvent);        // nicht verwenden, da sonst Fehler Guru Meditation Error: Core  1 panic'ed (Cache disabled but cached memory region accessed) aufritt IRAM_ATTR

    // ab hier Event an mqtt, dass Wifi initialisiert ist. Sonst stürzt WiFi ab, wenn mqtt zu schnell darauf zugreift
    if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
        if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_INITIALIZED) == 0) {
            xEventGroupSetBits(*_phEventGroupNetwork, EG_NETWORK_INITIALIZED);
            //Serial.println(F("[WiFi] Set Event EG_NETWORK_INITIALIZED"));
        }
    }


	for (;;) {
		if (WiFi.status() == WL_CONNECTED) {
            // wenn connected Bit noch nicht gesetzt, dann hier setzen
       		if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
                if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
        			xEventGroupSetBits(*_phEventGroupNetwork, EG_NETWORK_CONNECTED);
	        		Serial.println(F("[WiFi] Set Event EG_NETWORK_CONNECTED"));
                    
                    // if we have a status LED -> switch on
                    if (_pinLEDWifiStatus && _enableLEDs) {
                        digitalWrite(_pinLEDWifiStatus, HIGH);
                    }
                }
		    }
			vTaskDelay(pdMS_TO_TICKS(WIFI_CHECK_CONNECTION_STATE_TIME_MS));
			continue;
		}


		// Event, dass WiFi nicht verbunden ist
		if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
            if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) != 0) {
    			xEventGroupClearBits(*_phEventGroupNetwork, EG_NETWORK_CONNECTED);
	    		Serial.println(F("[WiFi] Reset Event EG_NETWORK_CONNECTED"));

                // if we have a status LED -> switch off
                if (_pinLEDWifiStatus) {
                    digitalWrite(_pinLEDWifiStatus, LOW);
                }
            }
		}

//		Serial.print(F("[WiFi] Connecting to "));
//		Serial.println(ssid);
		WiFi.begin(wifi_ssid, wifi_password);


		unsigned long startAttemptTime = millis();
        bool timedout = false;

		// Keep looping while we're not connected and haven't reached the timeout
		while (WiFi.status() != WL_CONNECTED && !timedout) {
				//millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
//			Serial.print(F("."));

            // if we have a status LED -> toggle
            if (_pinLEDWifiStatus) {
                digitalWrite(_pinLEDWifiStatus, digitalRead(_pinLEDWifiStatus) ? LOW : _enableLEDs ? HIGH : LOW);
            }

			vTaskDelay(pdMS_TO_TICKS(500));
            timedout = millis() - startAttemptTime > WIFI_TIMEOUT_MS;
			//taskYIELD();
		}

		// When we couldn't make a WiFi connection (or the timeout expired)
		// sleep for a while and then retry.
		if (timedout) {
//			Serial.println(F("[WiFi] FAILED"));

            // if we have a status LED -> switch off
            if (_pinLEDWifiStatus) {
                digitalWrite(_pinLEDWifiStatus, LOW);
           }

			vTaskDelay(pdMS_TO_TICKS(WIFI_RECOVER_TIME_MS));
            if (WiFi.status() != WL_CONNECTED) {
    			continue;
            }
		}

		if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
			xEventGroupSetBits(*_phEventGroupNetwork, EG_NETWORK_CONNECTED);
			Serial.println(F("[WiFi] Set Event EG_NETWORK_CONNECTED"));

            // if we have a status LED -> switch on
            if (_pinLEDWifiStatus && _enableLEDs) {
                digitalWrite(_pinLEDWifiStatus, HIGH);
            }
		}

		Serial.print(F("[WiFi] Connected: "));
		Serial.print(WiFi.localIP().toString());
        Serial.print(F(" to: "));
        Serial.println(WiFi.SSID());
	}
}



bool KSWiFiConnection::waitForInit(TickType_t xTicksToWait) {

	if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {

        // Waiting for Wifi-Connection
        if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
            // Serial.println(F("[WiFi] Wating for Event EG_NETWORK_CONNECTED"));
        
            EventBits_t eventGroupValue;
            eventGroupValue = xEventGroupWaitBits(*_phEventGroupNetwork, EG_NETWORK_CONNECTED, pdFALSE, pdTRUE, xTicksToWait);
            // Serial.println(F("[WiFi] Event EG_NETWORK_CONNECTED set"));
            if ((eventGroupValue & EG_NETWORK_CONNECTED) != EG_NETWORK_CONNECTED) {

                Serial.printf("No Connection to WiFi.\n");
                return false;
            }
        }
        return true;
    }
    return false;
}




void KSWiFiConnection::setStaticConfig(const char* pIP, const char* pGateway, const char* pSubnet, const char* pDns) {
    if (pIP && pGateway && pSubnet && pDns) {
        _staticIP.fromString(pIP);
        _staticGateway.fromString(pGateway);
        _staticSubnet.fromString(pSubnet);
        _staticDns.fromString(pDns);
        _useDHCP = false;

        // if we have a static configuration, use it
        if (!WiFi.config(_staticIP, _staticGateway, _staticSubnet, _staticDns)) {
            Serial.println("[WiFi] Failed to configure static IP address.");
        }
    }
}




// Darf nicht aufgerufen werden, wenn LittleFS aktiv ist. ansonsten Absürze
// Guru Meditation Error: Core  1 panic'ed (Cache disabled but cached memory region accessed)
// da in jeder Funktion, die von einem Interrupt aus aufgerufen wird, diese mit IRAM_ATTR definiert sein muss
void KSWiFiConnection::WiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            Serial.println("[WiFi] WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            Serial.println("[WiFi] Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            Serial.println("[WiFi] WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            Serial.println("[WiFi] WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("[WiFi] Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
//            Serial.println("[WiFi] Disconnected from WiFi access point");
//      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
//      xTimerStart(wifiReconnectTimer, 0);
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            Serial.println("[WiFi] Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.print("[WiFi] Obtained IP address: ");
            Serial.println(WiFi.localIP());
            //connectToMqtt();
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            Serial.println("[WiFi] Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            Serial.println("[WiFi] WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            Serial.println("[WiFi] WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            Serial.println("[WiFi] WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            Serial.println("[WiFi] WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("[WiFi] WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("[WiFi] WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("[WiFi] Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("[WiFi] Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            Serial.println("[WiFi] Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            Serial.println("[WiFi] Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            Serial.println("[WiFi] IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            Serial.println("[WiFi] Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Serial.println("[WiFi] Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Serial.println("[WiFi] Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Serial.println("[WiFi] Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            Serial.println("[WiFi] Obtained IP address");
            break;
        default:
            Serial.printf("[WiFi] event: %d\n", event);
            break;
    }
}










