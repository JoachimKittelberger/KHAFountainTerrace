# Project KHAFountainTerrace

## Anpassungen im Quellcode des Projekts
Um das Projekt sauber zu compilieren müssen die IP-Adressen und die WLAN-Einstellungen angepasst werden
1.	Datei include/AppCredentials.h.tpl in include/AppCredentials.h umbenennen und die Einstellungen in der Datei anpassen
2.	Datei data/credentials.js.tpl in data/credentials.js umbenennen und die Adresse des WebSockets an die Adresse des ESP32 anpassen
3.	Datei lib/KSCredentials/src/KSCredentials.h.tpl in lib/KSCredentials/src/KSCredentials.h umbenennen und die Einstellungen in der Datei anpassen
4.	Datei platformio_credentials.ini.tpl in platformio_credentials.ini umbenennen und die Einstellungen in der Datei anpassen

## Anpassung in Bibliothek esp32FOTA
5. In der Datei esp32FOTA.hpp der verwendeten Bibliothtke esp32FOTA muss die Reihenfolge der Erkennung angepasst werden, damit diese Bibliothek mit LittelFS funktioniert.
Hier muss zuerst LittleFS und dann erst SPIFFS geprüft werden, da SPIFFS.h wohl von einer anderen Lib includiert wird und deshalb immer zuerst erkannt wird
----

	// Changed: Swap the detection. First LittleFS and then SPIFFS
	// inherit includes from sketch, detect SPIFFS first for legacy support
	#if __has_include(<LittleFS.h>) || defined _LiffleFS_H_
		#if !defined(DISABLE_ALL_LIBRARY_WARNINGS)
		#pragma message "Using LittleFS for certificate validation"
		#endif
		#include <LittleFS.h>
		#define FOTA_FS &LittleFS
	#elif __has_include(<SPIFFS.h>) || defined _SPIFFS_H_
		#if !defined(DISABLE_ALL_LIBRARY_WARNINGS)
		#pragma message "Using SPIFFS for certificate validation"
		#endif
		#include <SPIFFS.h>
		#define FOTA_FS &SPIFFS
	#elif __has_include(<SD.h>) || defined _SD_H_


