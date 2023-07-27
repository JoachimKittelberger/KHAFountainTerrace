#ifndef _KSWIFICONNECTION_H
#define _KSWIFICONNECTION_H

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#elif defined(ESP32)
    #include <WiFi.h>
#else
#error "This isn't a ESP8266 or ESP32!"
#endif



class KSWiFiConnection {
    public:
        KSWiFiConnection(uint8_t pinLEDWifiStatus = 0);
        ~KSWiFiConnection();

        TaskHandle_t createConnection(EventGroupHandle_t *phEventGroupNetwork = NULL);
        bool waitForInit(TickType_t xTicksToWait = portMAX_DELAY);
 
        void setStaticConfig(const char* pIP, const char* pGateway, const char* pSubnet, const char* pDns);
        void enableStatusLEDs(bool enable = true) { _enableLEDs = enable; }

    private:
        void tKSWiFiConnection();
        
        static void WiFiEvent(WiFiEvent_t event);

        TaskHandle_t _htKSWiFiConnection;
        EventGroupHandle_t *_phEventGroupNetwork = NULL;


        bool _useDHCP = true;

        IPAddress _staticIP;      // statische IP des NodeMCU ESP8266
        IPAddress _staticGateway;        // IP-Adresse des Router
        IPAddress _staticSubnet;         // Subnetzmaske des Netzwerkes
        IPAddress _staticDns;              // DNS Server

        // Status LED for WiFi-Status. Blinking = connecting, on = connected, off = disconnected
        bool _enableLEDs = false;
        uint8_t _pinLEDWifiStatus = 0;
};




#endif // #define _KSWIFICONNECTION_H