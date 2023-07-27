#ifndef _CREDENTIALS_H
#define _CREDENTIALS_H


// TODO: copy this file to AppCredentials.h and use own secrets

#ifdef USE_LOCAL_AP
    // static IP Address of Brunnentest
    const char staticIP[] = "192.168.1.1";
    const char staticGateway[] = "192.168.1.1";
    const char staticSubnet[] = "255.255.255.0";
#else
    // static IP Address of Brunnentest
    const char staticIP[] = "192.168.1.1";
    const char staticGateway[] = "192.168.1.1";
    const char staticSubnet[] = "255.255.255.0";
    const char staticDNS[] = "192.168.1.1";
#endif

// IP-Address for Shelly to deactivate fountain
const char ShellyIPTerrasseLEDs[] = "192.168.1.10";

// Application specific
// TODO: diese Topics anpassen
const char mqttClientID[] = "MyFountainTerrace";


#endif          // #define _CREDENTIALS_H
