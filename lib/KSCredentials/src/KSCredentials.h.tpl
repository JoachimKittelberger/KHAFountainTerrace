#ifndef _KSCREDENTIALS_H
#define _KSCREDENTIALS_H


// Global definition of WLAN

// This is the SSID and password for your WIFI access point, change as desired
#ifdef USE_LOCAL_AP
const char* wifi_ssid = "ssid";
const char* wifi_password = "password";
#else
const char wifi_ssid[] = "ssid";
const char wifi_password[] = "password";
#endif



// fritzbox settings
const char fbUsername[] = "username";
const char fbPassword[] = "password";
const char fbIp[] = "192.168.1.1";



// This is the mqtt server and password
const char mqttServerIP[] = "192.168.1.2";
const char mqttUserName[] = "username";
const char mqttUserPassword[] = "password";


// This is the ftp server username and password
const char ftpUserName[] = "username";
const char ftpUserPassword[] = "password";




// this ist not stored in Preferences
// These are the homekit credentials and infos
const char hkManufacturer[] = "MyCompany";
const char hkDefaultSetupCode[] = "12345678";
const char HAPProtoInfoVersion[] = "1.0.0";


// This is the mqtt server and password
const char updateServerIP[] = "192.168.1.3";
const char updateUserName[] = "username";
const char updateUserPassword[] = "password";



#define SMTP_HOST "smtp.strato.de"
#define SMTP_PORT esp_mail_smtp_port_465 // port 465 is not available for Outlook.com





#endif          // #define _KSCREDENTIALS_H
