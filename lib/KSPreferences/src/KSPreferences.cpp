#include "KSPreferences.h"
//#include "KSCredentials.h"            // include global credential file

#include <nvs_flash.h>


/*
extern const char wifi_ssid[];
extern const char wifi_password[];

extern const char fbUsername[];
extern const char fbPassword[];
extern const char fbIp[];

extern const char mqttServerIP[];
extern const char mqttUserName[];
extern const char mqttUserPassword[];

extern const char ftpUserName[];
extern const char ftpUserPassword[];

*/



void KSPreferences::putKSValues() {
    eraseNVSMemory();               // erase complete NVS memory

/*
    // credenntials for wifi
    begin("wifi", false);
	putString("ssid", wifi_ssid); 
	putString("password", wifi_password);
    end();

    // credentials for fritzbox access
    begin("fritzbox", false);
	putString("username", fbUsername); 
	putString("password", fbPassword);
	putString("ip", fbIp);
    end();

    // credentials for mqtt server
    begin("mqtt", false);
	putString("ip", mqttServerIP); 
	putString("username", mqttUserName);
	putString("password", mqttUserPassword);
    end();

    // credentials for ftp server
    begin("ftp", false);
	putString("username", ftpUserName); 
	putString("password", ftpUserPassword);
    end();
    */
}



// erase the complete NVS-Memory
void KSPreferences::eraseNVSMemory() {
    nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init(); // initialize the NVS partition.
}


