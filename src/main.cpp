/**
 * @file main.cpp
 * 
 * @brief control of pump and WS2812b-LEDs of fontain on terrace
 * 
 * @details
 * 
 * @see Hardware description in "Schaltplan BrunnenTerrasse.pdf"
 *   - me-no-dev/AsyncTCP@^1.1.1										; ESPAsyncWebServer
 *   - https://github.com/me-no-dev/ESPAsyncWebServer.git @ ^1.2.3		; ESPAsyncWebServer
 *   - bblanchon/ArduinoJson @ ^6.21.3									; JSON
 * 
 * @author Joachim Kittelberger <jkittelberger@kibesoft.de>
 * @date 01.07.2022
 * @version 1.00
 *
 * @todo
 *   + store last settings in preferences
 *   + set default values to other values
 *
 *   - Use KSOTA
 *   - Use KSMQTTConnection
 *   - Change Partition-Table with CSV (more size and OTA)
 *   - Add FileHeaders to all files
 *   - add more effects (see links in KSWS2812B.h in todo-section)
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


//#define LOG_SERIAL
//#define LOG_TELNET
#define LOG_SERIAL_AND_TELNET

#define MQTT_MAX_SUBSCRIPTIONS 3

// just for Test use local access point and not home network
//#define USE_LOCAL_AP

#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>


#include "AppInfo.h"
#include "AppCredentials.h"			// global App Credentials: must be copied from AppCredentials.h.tpl
#include "KSCredentials.h"			// global KSLibrariesCredentials: must bei copied from KSCredentials.h.tpl

#include "KSUtilities.h"
#include "KSFileSystem.h"
#include "KSTelnetServer2.h"


#include <driver/ledc.h>		// for PWM output ledc-functions
// Pins for controlling motor PWM with own MOSEFT-Driver-Modul
const int MotorPWMPin = 25;
#define MOTORPWM_FREQ		5000
#define LEDC_CHANNEL_MOTORPWM LEDC_CHANNEL_0


// WS2812B-LEDs
#include "KSWS2812B.h"
const int NeopixelPinBrunnen = 14;
#define NUMPIXELSBRUNNEN 84			// LEDs of Brunnen
KSWS2812B lightsBrunnen(NUMPIXELSBRUNNEN, NeopixelPinBrunnen);

const int NeopixelPinStufe = 12;
#define NUMPIXELSSTUFE 107			// LEDs of Stufe
KSWS2812B lightsStufe(NUMPIXELSSTUFE, NeopixelPinStufe);

const int NeopixelPinRing = 13;
#define NUMPIXELSRING 12			// LEDs of Ring in Brunnen
KSWS2812B lightsRing(NUMPIXELSRING, NeopixelPinRing);


// Temperature in housing
#include "KSBME280.h"
#define READ_TEMPERATURE_INTERVALL 1*60*1000	// read temperature every 1 min
#define KSBME280_ADDRESS 0x76
const int SDAPin = 18;
const int SCLPin = 17;
KSBME280 bme280(KSBME280_ADDRESS, &Wire);
float temperature = 0;
float humidity = 0;
bool bTemperatureToHigh = false;


// ModeRelais for switching between solar and power mode
const int ModeRelais1Pin = 21;
const int ModeRelais2Pin = 22;

const int SpeedControlPotiPin = 32;			// Nur zum Test fürs einlesen von Werten

// we controll the power of the LEDs and Fontain with a shelly at 220V side
#include "KSShelly.h"


#include "KSEventGroupNetwork.h"
EventGroupHandle_t hEventGroupNetwork = NULL;

#include "KSWiFiConnection.h"
KSWiFiConnection wifi;

#include "KSNTPClient.h"
KSNTPClient ntp;

#include "KSFTPServer.h"
KSFTPServer ftp;


//KSMQTTConnection mqtt;
//KSOTA ota(PROJECT_NAME);				// Enable OTA
//KSResetController resetController(2*24*60*60);		// reset the device every 2d

//KSHomeKit homeKit(PROJECT_NAME, SW_VERSION);  // Projectname wird ignoriert. Wird von homeSpan Service DisplayName genommen



#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


#include "GlobalDefinitions.h"
BrunnenData brunnenTerrace;






#include "KSLogger.h"
//#define LOGGER Serial		// TODO
//#define LOG_LEVEL LOG_LEVEL_TRACE
//#define LOG_LEVEL LOG_LEVEL_DEBUG


#define SAVE_SETTINGS_INTERVALL 5*60*1000		// save last settings all 5 min if something has changed
#define PREFKEY_FOUNTAIN_TERRACE "fountainterrace"
bool bSettingsChanged = false;

#include "KSPreferences.h"			// wird nur benötigt, wenn in eigener Datei ausgelagert
bool saveCurrentDataToMemory(BrunnenData* pBrunnen);
bool loadCurrentDataFromMemory(BrunnenData* pBrunnen);
void deleteDataFromMemory();


bool saveCurrentDataToMemory(BrunnenData* pBrunnen) {
	KSPreferences prefs;
	if (pBrunnen && prefs.begin(PREFKEY_FOUNTAIN_TERRACE, false)) {

		// names must be max 15 chars
		// save fontain data
		prefs.putBool("issolaron", pBrunnen->fontain.isSolarOn);
		prefs.putBool("isfontainon", pBrunnen->fontain.isFontainOn);
		prefs.putInt("height", pBrunnen->fontain.height);
			
		// save ledBrunnen data
		prefs.putBool("brison", pBrunnen->ledBrunnen.isOn);
		prefs.putInt("brbrightness", pBrunnen->ledBrunnen.brightness);
		prefs.putUInt("brcolor", pBrunnen->ledBrunnen.color);
		prefs.putInt("brlighteffect", pBrunnen->ledBrunnen.lightEffect);
		prefs.putInt("brlespeed", pBrunnen->ledBrunnen.lightEffectSpeed);
		
		// save ledRing data
		prefs.putBool("riison", pBrunnen->ledRing.isOn);
		prefs.putInt("ribrightness", pBrunnen->ledRing.brightness);
		prefs.putUInt("ricolor", pBrunnen->ledRing.color);
		prefs.putInt("rilighteffect", pBrunnen->ledRing.lightEffect);
		prefs.putInt("rilespeed", pBrunnen->ledRing.lightEffectSpeed);

		// save ledStufe data
		prefs.putBool("stison", pBrunnen->ledStufe.isOn);
		prefs.putInt("stbrightness", pBrunnen->ledStufe.brightness);
		prefs.putUInt("stcolor", pBrunnen->ledStufe.color);
		prefs.putInt("stlighteffect", pBrunnen->ledStufe.lightEffect);
		prefs.putInt("stlespeed", pBrunnen->ledStufe.lightEffectSpeed);

		prefs.end();
		return true;
	}
	prefs.end();
	return false;
}


bool loadCurrentDataFromMemory(BrunnenData* pBrunnen) {
	KSPreferences prefs;
	if (pBrunnen && prefs.begin(PREFKEY_FOUNTAIN_TERRACE, true)) {
		// names must be max 15 chars
		// load fontain data
		pBrunnen->fontain.isSolarOn = prefs.getBool("issolaron", false);
		pBrunnen->fontain.isFontainOn = prefs.getBool("isfontainon", false);
		pBrunnen->fontain.height = prefs.getInt("height", 150);
		pBrunnen->fontain.height = constrain(pBrunnen->fontain.height, 0, 255);
		
		// load ledBrunnen data
		pBrunnen->ledBrunnen.isOn = prefs.getBool("brison", false);
		pBrunnen->ledBrunnen.brightness = prefs.getInt("brbrightness", 100);
		pBrunnen->ledBrunnen.brightness = constrain(pBrunnen->ledBrunnen.brightness, 0, 255);
		pBrunnen->ledBrunnen.color = prefs.getUInt("brcolor", 0x0000FF);
		int le = prefs.getInt("brlighteffect", LightEffect::LEBlack);
		le = constrain(le, 0, LightEffect::LENumOfLightEffects - 1);
		pBrunnen->ledBrunnen.lightEffect = (LightEffect)le;
		pBrunnen->ledBrunnen.lightEffectSpeed = prefs.getInt("brlespeed", 100);
		pBrunnen->ledBrunnen.lightEffectSpeed = constrain(pBrunnen->ledBrunnen.lightEffectSpeed, 20, 2000);

		// load ledRing data
		pBrunnen->ledRing.isOn = prefs.getBool("riison", false);
		pBrunnen->ledRing.brightness = prefs.getInt("ribrightness", 100);
		pBrunnen->ledRing.brightness = constrain(pBrunnen->ledRing.brightness, 0, 255);
		pBrunnen->ledRing.color = prefs.getUInt("ricolor", 0x0000FF);
		le = prefs.getInt("rilighteffect", LightEffect::LEBlack);
		le = constrain(le, 0, LightEffect::LENumOfLightEffects - 1);
		pBrunnen->ledRing.lightEffect = (LightEffect)le;
		pBrunnen->ledRing.lightEffectSpeed = prefs.getInt("rilespeed", 100);
		pBrunnen->ledRing.lightEffectSpeed = constrain(pBrunnen->ledRing.lightEffectSpeed, 20, 4000);

		// load ledStufe data
		pBrunnen->ledStufe.isOn = prefs.getBool("stison", false);
		pBrunnen->ledStufe.brightness = prefs.getInt("stbrightness", 100);
		pBrunnen->ledStufe.brightness = constrain(pBrunnen->ledStufe.brightness, 0, 255);
		pBrunnen->ledStufe.color = prefs.getUInt("stcolor", 0x0000FF);
		le = prefs.getInt("stlighteffect", LightEffect::LEBlack);
		le = constrain(le, 0, LightEffect::LENumOfLightEffects - 1);
		pBrunnen->ledStufe.lightEffect = (LightEffect)le;
		pBrunnen->ledStufe.lightEffectSpeed = prefs.getInt("stlespeed", 100);
		pBrunnen->ledStufe.lightEffectSpeed = constrain(pBrunnen->ledStufe.lightEffectSpeed, 20, 2000);

		bSettingsChanged = false;
		prefs.end();
		return true;
	}
	prefs.end();
	return false;
}


void deleteDataFromMemory() {
	KSPreferences prefs;

	if (prefs.begin(PREFKEY_FOUNTAIN_TERRACE, false)) {
		prefs.clear();		// alle Keys unterhalb Namespace löschen

		// oder keys einzeln löschen
		// names must be max 15 chars
		// prefs.remove("issolaron");	// remove key from namespace
		bSettingsChanged = true;
	}
	
	prefs.end();
}






// ****************************************************************************
// Functions for handling mode relais
void initModeRelaisModul() {
	pinMode(ModeRelais1Pin, OUTPUT);
	pinMode(ModeRelais2Pin, OUTPUT);
	switchToSolarMode();			// default is solar mode
}


void switchToSolarMode(bool bSolarMode) {
    if (!bSolarMode) {
        switchToPowerMode();
	} else {
		digitalWrite(ModeRelais1Pin, true);		// ausschalten
		digitalWrite(ModeRelais2Pin, true);
		brunnenTerrace.fontain.isSolarOn = true;
		bSettingsChanged = true;
    }
}


void switchToPowerMode() {
	digitalWrite(ModeRelais1Pin, false);	// einschalten
	digitalWrite(ModeRelais2Pin, false);
	brunnenTerrace.fontain.isSolarOn = false;
	bSettingsChanged = true;
}


bool isInSolarMode() {
	if (digitalRead(ModeRelais1Pin) && digitalRead(ModeRelais2Pin))
		return true;		// we are in Solar mode
	return false;
}
// ****************************************************************************


// ****************************************************************************
// Functions for handling Power Mosfet fontain
int lastFontainHeight = 0;

void initFontain() {
	pinMode(MotorPWMPin, OUTPUT);
	digitalWrite(MotorPWMPin, false);
	ledcSetup(LEDC_CHANNEL_MOTORPWM, MOTORPWM_FREQ, LEDC_TIMER_8_BIT);
	ledcAttachPin(MotorPWMPin, LEDC_CHANNEL_MOTORPWM);
	lastFontainHeight = 150;
}


void setFontainOn(bool bOn) {
	if (bOn) {
		ledcWrite(LEDC_CHANNEL_MOTORPWM, lastFontainHeight);
		brunnenTerrace.fontain.isFontainOn = true;
		bSettingsChanged = true;
	} else {
		setFontainOff();
	}
}


void setFontainOff() {
	ledcWrite(LEDC_CHANNEL_MOTORPWM, 0);
	brunnenTerrace.fontain.isFontainOn = false;
	bSettingsChanged = true;
}


bool isFontainOn() {
	if (ledcRead(LEDC_CHANNEL_MOTORPWM) == 0) {
		return false;
	}
	return true;
}


void setFontainHeight(int height) {        // maximum hight = 255. Minimum Hight = 150? Eventuell hier in % arbeiten
	// set the velocity on MOSFET-Module
	ledcWrite(LEDC_CHANNEL_MOTORPWM, height);
	lastFontainHeight = height;
	brunnenTerrace.fontain.height = height;
	bSettingsChanged = true;
}


int getFontainHeight() {
	return lastFontainHeight;
}

// ****************************************************************************


// switch of the power via the shelly in the device
void switchPowerOff() {
	// Lights Terrasse Shelly off
	
	KSShelly shelly(ShellyIPTerrasseLEDs);
	bool bLastState = shelly.setState(false, 0);
	if (shelly.hasLastError()) {
		LOGGER.printf("Error in Shelly set to false.\n");
	}
	LOGGER.printf("Shelly set to false. Last State was: %s\n", bLastState ? "true" : "false");
}

/*
// callback function called from homespan
// muss anstatt WiFiConnection verwendet werden
void onWifiEstablished(){
	Serial.print("wifiEstablished: IN CALLBACK FUNCTION\n\n");

	// ab hier Event, dass Wifi initialisiert ist. Sonst stürzt WiFi ab, wenn mqtt zu schnell darauf zugreift
	if (hEventGroupNetwork) {
		if ((xEventGroupGetBits(hEventGroupNetwork) & EG_NETWORK_INITIALIZED) == 0) {
			xEventGroupSetBits(hEventGroupNetwork, EG_NETWORK_INITIALIZED);
		}
	}

  	// wenn connected Bit noch nicht gesetzt, dann hier setzen
	CheckWifiEstablishedState();
}



// check the wifi state and set the Bit in hEventGroupNetork because the on WifiEstablished()-callback isn't called every time
void CheckWifiEstablishedState() {
	// wenn connected Bit noch nicht gesetzt, dann hier setzen, wenn Wifi-Status = WL_CONNECTED
	if (hEventGroupNetwork) {
		if ((xEventGroupGetBits(hEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
			if (WiFi.status() == WL_CONNECTED) {
				xEventGroupSetBits(hEventGroupNetwork, EG_NETWORK_CONNECTED);
			}
		}
	}
}
*/

void createJSONMessage(char* pOutput, size_t len) {
	// Serialize JSON. JSON mit aktuellen Daten zusammenbauen
	StaticJsonDocument<1024> JSONDoc;

	// create data for brunnen
	JsonObject ledbrunnen = JSONDoc.createNestedObject("ledbrunnen");
	ledbrunnen["mode"] = brunnenTerrace.ledBrunnen.isOn ? "on" : "off";
	ledbrunnen["brightness"] = brunnenTerrace.ledBrunnen.brightness;
	
	uint32_t color = brunnenTerrace.ledBrunnen.color;
	char szColorBrunnen[10] = "#ffffff";
	snprintf(szColorBrunnen, sizeof(szColorBrunnen), "#%06x", color);
	ledbrunnen["colorRGB"] = szColorBrunnen;
	ledbrunnen["lighteffect"] = brunnenTerrace.ledBrunnen.lightEffect;
	ledbrunnen["lespeed"] = brunnenTerrace.ledBrunnen.lightEffectSpeed;


	// create data for ledring
	JsonObject ledring = JSONDoc.createNestedObject("ledring");
	ledring["mode"] = brunnenTerrace.ledRing.isOn ? "on" : "off";
	ledring["brightness"] = brunnenTerrace.ledRing.brightness;
	
	color = brunnenTerrace.ledRing.color;
	char szColorRing[10] = "#ffffff";
	snprintf(szColorRing, sizeof(szColorRing), "#%06x", color);
	ledring["colorRGB"] = szColorRing;
	ledring["lighteffect"] = brunnenTerrace.ledRing.lightEffect;
	ledring["lespeed"] = brunnenTerrace.ledRing.lightEffectSpeed;


	// create data for ledstufe
	JsonObject ledstufe = JSONDoc.createNestedObject("ledstufe");
	ledstufe["mode"] = brunnenTerrace.ledStufe.isOn ? "on" : "off";
	ledstufe["brightness"] = brunnenTerrace.ledStufe.brightness;
	
	color = brunnenTerrace.ledStufe.color;
	char szColorStufe[10] = "#ffffff";
	snprintf(szColorStufe, sizeof(szColorStufe), "#%06x", color);
	ledstufe["colorRGB"] = szColorStufe;
	
	ledstufe["lighteffect"] = brunnenTerrace.ledStufe.lightEffect;
	ledstufe["lespeed"] = brunnenTerrace.ledStufe.lightEffectSpeed;


	// create data for fontain
	JsonObject fontain = JSONDoc.createNestedObject("fontain");
	fontain["solar"] = brunnenTerrace.fontain.isSolarOn ? "on" : "off";
	fontain["mode"] = brunnenTerrace.fontain.isFontainOn ? "on" : "off";
	fontain["height"] = brunnenTerrace.fontain.height;


	// create data for cmd
	JsonObject cmd = JSONDoc.createNestedObject("cmd");
	cmd["power"] = "on";

	// create data for status
	JsonObject status = JSONDoc.createNestedObject("status");
	status["temperature"] = temperature;
	status["humidity"] = humidity;

	serializeJson(JSONDoc, pOutput, len);
}



// send info about current switch state to WebSocketClients
void notifyClientWithAllStates(AsyncWebSocketClient *client) {
	char output[1024];
	createJSONMessage(output, sizeof(output));

	//LOGGER.print("WebSocket Data sent: ");
	//LOGGER.println(output);
	if (client) {
		client->text(output);			// send just to one client
	} else {
		ws.textAll(output);				// notify all clients
	}
}



/*
// JSON-Document für WebService und Kommunikation mit html-page und mqtt
{
    "ledbrunnen": {
        "mode":"on",
        "brightness":100,
		"colorRGB":"#ff0055",
        "lighteffect":"LEDBlack",       // hier eventuell mit Zahlen arbeiten oder Liste mit FLASHHELPER Array und Texten durchgehen und Index ermitteln
        "lespeed":100
    },
    "ledring": {
    ...
    },

    "ledstufe: {
    ...
    },
    
    "fontain": {
        "solar":"on",
        "mode":"on",
        "height":150
    },
	"cmd": {
		"power":"off"
	},
	"status": {
		"temperature":35.4,
		"humidity": 70.1
	}
}
*/


void handleJSONMessage(const char* data, size_t len) {

	StaticJsonDocument<1024> JSONDoc; //Memory pool
	auto error = deserializeJson(JSONDoc, (const char*)data, len);
	if (error) {
		LOGGER.print(F("Parsing failed. Error: "));
		LOGGER.println(error.f_str());
	} else {

		JsonObject brunnen = JSONDoc["ledbrunnen"];
		if (!brunnen.isNull()) {
			LOGGER.printf("detected JSON brunnen:");
			
			const char* brunnen_mode = brunnen["mode"]; // "on"
			if (brunnen_mode) {
				LOGGER.printf(" mode: %s", brunnen_mode);
				if (strcmp(brunnen_mode, "on") == 0) {
					lightsBrunnen.setLightsOn();
					brunnenTerrace.ledBrunnen.isOn = true;
				} else if (strcmp(brunnen_mode, "off") == 0) {
					lightsBrunnen.setLightsOff();
					brunnenTerrace.ledBrunnen.isOn = false;
				}
			}

			if (brunnen.containsKey("brightness")) {
				int brunnen_brightness = brunnen["brightness"]; // 100
				brunnen_brightness = constrain(brunnen_brightness, 0, 255);
				lightsBrunnen.setBrightness((uint8_t)brunnen_brightness);
				lightsBrunnen.updateLights();
				brunnenTerrace.ledBrunnen.brightness = (uint8_t)brunnen_brightness;
				LOGGER.printf(" brightness: %d", brunnen_brightness);
			}

			const char* brunnen_colorRGB = brunnen["colorRGB"]; // "#ff2600"
			if (brunnen_colorRGB) {
				LOGGER.printf("Received new color: %s", brunnen_colorRGB);
				char* pEnd;
				long li = strtol(brunnen_colorRGB+1, &pEnd, 16);
				LOGGER.printf(" value: %ld", li);
				
				lightsBrunnen.setColor((uint32_t)li);
				lightsBrunnen.updateLights();
				brunnenTerrace.ledBrunnen.color = (uint32_t)li;
			}

			if (brunnen.containsKey("lighteffect")) {
				int brunnen_lighteffect = brunnen["lighteffect"]; // index
				brunnen_lighteffect = constrain(brunnen_lighteffect, 0, LightEffect::LENumOfLightEffects - 1);
				lightsBrunnen.setLightEffect((LightEffect)brunnen_lighteffect);
				brunnenTerrace.ledBrunnen.lightEffect = (LightEffect)brunnen_lighteffect;
			}

			if (brunnen.containsKey("lespeed")) {
				int brunnen_lespeed = brunnen["lespeed"]; // 100
				brunnen_lespeed = constrain(brunnen_lespeed, 20, 2000);
				lightsBrunnen.setAnimationTimeMS(brunnen_lespeed);
				brunnenTerrace.ledBrunnen.lightEffectSpeed = brunnen_lespeed;
				LOGGER.printf(" lespeed: %d", brunnen_lespeed);
			}
			LOGGER.println();
		}


		JsonObject ledring = JSONDoc["ledring"];
		if (!ledring.isNull()) {
			LOGGER.printf("detected JSON ledring:");

			const char* ledring_mode = ledring["mode"]; // "on"
			if (ledring_mode) {
				LOGGER.printf(" mode: %s", ledring_mode);
				if (strcmp(ledring_mode, "on") == 0) {
					lightsRing.setLightsOn();
					brunnenTerrace.ledRing.isOn = true;
				} else if (strcmp(ledring_mode, "off") == 0) {
					lightsRing.setLightsOff();
					brunnenTerrace.ledRing.isOn = false;
				}
			}

			if (ledring.containsKey("brightness")) {
				int ledring_brightness = ledring["brightness"]; // 100
				ledring_brightness = constrain(ledring_brightness, 0, 255);
				lightsRing.setBrightness((uint8_t)ledring_brightness);
				lightsRing.updateLights();
				brunnenTerrace.ledRing.brightness = (uint8_t)ledring_brightness;
				LOGGER.printf(" brightness: %d", ledring_brightness);
			}

			const char* ledring_colorRGB = ledring["colorRGB"]; // "#ff2600"
			if (ledring_colorRGB) {
				LOGGER.printf("Received new color: %s", ledring_colorRGB);
				char* pEnd;
				long li = strtol(ledring_colorRGB+1, &pEnd, 16);
				LOGGER.printf(" value: %ld", li);
				
				lightsRing.setColor((uint32_t)li);
				lightsRing.updateLights();
				brunnenTerrace.ledRing.color = (uint32_t)li;
			}

			if (ledring.containsKey("lighteffect")) {
				int ledring_lighteffect = ledring["lighteffect"]; // index
				ledring_lighteffect = constrain(ledring_lighteffect, 0, LightEffect::LENumOfLightEffects - 1);
				lightsRing.setLightEffect((LightEffect)ledring_lighteffect);
				brunnenTerrace.ledRing.lightEffect = (LightEffect)ledring_lighteffect;
			}

			if (ledring.containsKey("lespeed")) {
				int ledring_lespeed = ledring["lespeed"]; // 100
				ledring_lespeed = constrain(ledring_lespeed, 20, 4000);
				lightsRing.setAnimationTimeMS(ledring_lespeed);
				brunnenTerrace.ledRing.lightEffectSpeed = ledring_lespeed;
				LOGGER.printf(" lespeed: %d", ledring_lespeed);
			}

			LOGGER.println();
		}


		JsonObject ledstufe = JSONDoc["ledstufe"];
		if (!ledstufe.isNull()) {
			LOGGER.printf("detected JSON ledstufe:");

			const char* ledstufe_mode = ledstufe["mode"]; // "on"
			if (ledstufe_mode) {
				LOGGER.printf(" mode: %s", ledstufe_mode);
				if (strcmp(ledstufe_mode, "on") == 0) {
					lightsStufe.setLightsOn();
					brunnenTerrace.ledStufe.isOn = true;
				} else if (strcmp(ledstufe_mode, "off") == 0) {
					lightsStufe.setLightsOff();
					brunnenTerrace.ledStufe.isOn = false;
				}
			}

			if (ledstufe.containsKey("brightness")) {
				int ledstufe_brightness = ledstufe["brightness"]; // 100
				ledstufe_brightness = constrain(ledstufe_brightness, 0, 255);
				lightsStufe.setBrightness((uint8_t)ledstufe_brightness);
				lightsStufe.updateLights();
				brunnenTerrace.ledStufe.brightness = (uint8_t)ledstufe_brightness;
				LOGGER.printf(" brightness: %d", ledstufe_brightness);
			}

			const char* ledstufe_colorRGB = ledstufe["colorRGB"]; // "#ff2600"
			if (ledstufe_colorRGB) {
				LOGGER.printf("Received new color: %s", ledstufe_colorRGB);
				char* pEnd;
				long li = strtol(ledstufe_colorRGB+1, &pEnd, 16);
				LOGGER.printf(" value: %ld", li);
				
				lightsStufe.setColor((uint32_t)li);
				lightsStufe.updateLights();
				brunnenTerrace.ledStufe.color = (uint32_t)li;
			}

			if (ledstufe.containsKey("lighteffect")) {
				int ledstufe_lighteffect = ledstufe["lighteffect"]; // index
				ledstufe_lighteffect = constrain(ledstufe_lighteffect, 0, LightEffect::LENumOfLightEffects - 1);
				lightsStufe.setLightEffect((LightEffect)ledstufe_lighteffect);
				brunnenTerrace.ledStufe.lightEffect = (LightEffect)ledstufe_lighteffect;
			}

			if (ledstufe.containsKey("lespeed")) {
				int ledstufe_lespeed = ledstufe["lespeed"]; // 100
				ledstufe_lespeed = constrain(ledstufe_lespeed, 20, 2000);
				lightsStufe.setAnimationTimeMS(ledstufe_lespeed);
				LOGGER.printf(" lespeed: %d", ledstufe_lespeed);
				brunnenTerrace.ledStufe.lightEffectSpeed = ledstufe_lespeed;
			}

			LOGGER.println();
		}


		JsonObject fontain = JSONDoc["fontain"];
		if (!fontain.isNull()) {
			LOGGER.print("detected JSON fontain:");

			if (fontain.containsKey("solar")) {
				const char* fontain_solar = fontain["solar"]; // "on"
				LOGGER.printf(" solar: %s", fontain_solar);
				if (strcmp(fontain_solar, "on") == 0) {
					if (!isInSolarMode())
						switchToSolarMode();
				} else if (strcmp(fontain_solar, "off") == 0) {
					if (isInSolarMode())
						switchToPowerMode();
				}
			}

			if (fontain.containsKey("mode")) {
				const char* fontain_mode = fontain["mode"]; // "on"
				LOGGER.printf(" mode: %s", fontain_mode);
				if (strcmp(fontain_mode, "on") == 0) {
					setFontainOn();
				} else if (strcmp(fontain_mode, "off") == 0) {
					setFontainOff();
				}
			}

			if (fontain.containsKey("height")) {
				int fontain_height = fontain["height"]; // 150
				fontain_height = constrain(fontain_height, 0, 255);
				LOGGER.printf(" height: %d", fontain_height);
				setFontainHeight(fontain_height);
			}
			LOGGER.println();
		}

		// power off the device
		JsonObject cmd = JSONDoc["cmd"];
		if (!cmd.isNull()) {
			LOGGER.print("detected JSON cmd:");

			if (cmd.containsKey("power")) {
				const char* cmd_power = cmd["power"]; // "on"
				LOGGER.printf(" power: %s", cmd_power);
				if (strcmp(cmd_power, "off") == 0) {
					switchPowerOff();
				}
			}
			LOGGER.println();
		}

		// power off the device
		JsonObject status = JSONDoc["status"];
		if (!status.isNull()) {
			LOGGER.print("detected JSON status:");

			if (cmd.containsKey("temperature")) {
				float status_temperature = status["temperature"];		// do nothing because value come from ESP
				LOGGER.printf(" temperature: %f", status_temperature);
			}
			if (cmd.containsKey("humidity")) {
				float status_humidity = status["humidity"];	// do nothing because value come from ESP
				LOGGER.printf(" humidity: %f", status_humidity);
			}
			LOGGER.println();
		}

		bSettingsChanged = true;			// something has changed
	}
}



// handle received WebSocketChange message and switch the power states
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  	AwsFrameInfo *info = (AwsFrameInfo*)arg;
  	if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

		handleJSONMessage((const char *)data, len);

		// Update all clients with this value
  		//ws.textAll(String((const char*)data));
  		//ws.textAll((const char*)data, len);		// because of missing '\0' in data
		
		// update the complete information via webservice
		notifyClientWithAllStates(NULL);		// notify all clients
	}
}





void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	switch (type) {
		case WS_EVT_CONNECT:
			LOGGER.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
			notifyClientWithAllStates(client);		// notify new client with current state
			break;
		case WS_EVT_DISCONNECT:
			LOGGER.printf("WebSocket client #%u disconnected\n", client->id());
			break;
		case WS_EVT_DATA:
			//LOGGER.print("WebSocket Data received: ");
		    //for (int i=0; i < len; i++) {
	    	//      LOGGER.print((char) data[i]);
	    	//}
	 		//LOGGER.println();
			handleWebSocketMessage(arg, data, len);
			break;
		case WS_EVT_PONG:
		case WS_EVT_ERROR:
			break;
	}
}



// Replaces placeholder with button section in your web page
String processor(const String& var){
	//LOGGER.printf("Processor replaces %s\n", var.c_str());

	if (var.equals("LightEffectOptions")) {
		//LOGGER.printf("Processor Detected LightEffectOptions.\n");

		// erzeugt werden muss:  <option value="free">Free</option>
		String retVal = "";
		for (int i = 0; i < LightEffect::LENumOfLightEffects; i++) {
			retVal += "<option value=\"";
			retVal += i;
			retVal += "\">";
			retVal += lightEffects[i].pEffectName;
			retVal += "</option>\n";
		}
		return retVal;
	}

	if (var.equals("Version")) {
		//LOGGER.printf("Processor Detected Version.\n");
		String retVal = String(SW_VERSION);
		return retVal;
	}

	return String(var);
	//return String();
}



void notFound(AsyncWebServerRequest *request) {
   	LOGGER.println("notFound" + request->url());
    request->send(404, "text/plain", "Not found");
}





char * listESPStateJSON() {
	const size_t bufferSize = JSON_OBJECT_SIZE(10) + 1500;
	DynamicJsonDocument jsonBuffer(bufferSize);
	JsonObject root = jsonBuffer.to<JsonObject>();
	root["heap"] = ESP.getFreeHeap();
	root["sketch_size"] = ESP.getSketchSize();
	root["free_sketch_space"] = ESP.getFreeSketchSpace();
	root["flash_chip_size"] = ESP.getFlashChipSize();
	root["sdk_version"] = ESP.getSdkVersion();
	root["cpu_freq"] = ESP.getCpuFreqMHz();
	root["chip_model"] = ESP.getChipModel();
	root["chip_cores"] = ESP.getChipCores();
	root["chip_revision"] = ESP.getChipRevision();

	uint16_t msg_len = measureJson(root) + 1;
	char * buffer = (char *) malloc(msg_len);
	serializeJson(root, buffer, msg_len);
	jsonBuffer.clear();
	return buffer;
}



void onTelnetReadCommand(char* szCommand) {

	if (strcasecmp(szCommand, "Info") == 0) {
		LOGGER.printf("Project: %s Version: %s\n", PROJECT_NAME, SW_VERSION);
		LOGGER.printf("Temperature: %.02f°C, Humidity: %.01f%%\n", temperature, humidity);

		char * buffer = listESPStateJSON();
		LOGGER.println(buffer);
		free(buffer);
	}
	else if (strcasecmp(szCommand, "Erase") == 0) {
		LOGGER.println("Erase all Data");
		deleteDataFromMemory();
//		homeKit.EraseFlashData();
	}
	else if (strcasecmp(szCommand, "Save") == 0) {
		LOGGER.print("Save Data ...");
		if (saveCurrentDataToMemory(&brunnenTerrace)) {
			bSettingsChanged = false;
			LOGGER.println(" Succeeded!");
		} else {
			LOGGER.println(" Error in Saving!");
		}
	}

	else if (strcasecmp(szCommand, "Fontain On") == 0) {
		LOGGER.println("Set Fontain On");
		setFontainOn();
	}
	else if (strcasecmp(szCommand, "Fontain Off") == 0) {
		LOGGER.println("Set Fontain Off");
		setFontainOff();
	}
	else if (strStartsWith(szCommand, "Fontain Height")) {		// use: "Fontain Height=150"
		// get String after "Fontain Height"
		int height = -1;
		char* pChar = strrchr(szCommand, '=');
		if (pChar) {
			if (strlen(pChar) > 2)
				pChar++;		// go behind the '=' sign
			height = atoi(pChar);
		}
		if (height > -1)
			setFontainHeight(height);
		LOGGER.printf("Set Fontain Height to %d\n", height);
	}

	else if (strcasecmp(szCommand, "SolarMode") == 0) {
		LOGGER.println("Switch to Solar Mode");
		switchToSolarMode();
	}
	else if (strcasecmp(szCommand, "PowerMode") == 0) {
		LOGGER.println("Switch to Power Mode");
		switchToPowerMode();
	}
	else {
		LOGGER.printf("[telnet] Unkown command: %s\n", szCommand);
	}
}




// callback function
void onReset() {
  // is called before Reset of ESP32
  Serial.println("Reseting ESP32 ...");
  Serial.flush();
  vTaskDelay(pdMS_TO_TICKS(100));
}



// damit der Watchdog bei langen http-Aufrufen nicht zuschlägt, manuell auf höhere Zeit setzen
#include "soc/rtc_wdt.h"
// TODO in setup()
//rtc_wdt_protect_off();    // Turns off the automatic wdt service
//rtc_wdt_enable();         // Turn it on manually
//rtc_wdt_set_time(RTC_WDT_STAGE0, 20000);  // Define how long you desire to let dog wait.
// TODO in loop()
//rtc_wdt_feed(); // feed the Watchdog




void setup(){
 	initModeRelaisModul();

	// Analog-Input for Poti on Pin 26
	pinMode(SpeedControlPotiPin, INPUT);

	// init own MOSFET-Modul
	initFontain();
	setFontainOff();

	// Serial port for debugging purposes
	Serial.begin(115200);
#if ARDUINO_USB_CDC_ON_BOOT
    Serial.setTxTimeoutMs(0);
    vTaskDelay(pdMS_TO_TICKS(100));
#else
    while (!Serial)
        yield();
#endif
  	//vTaskDelay(pdMS_TO_TICKS(500));  // let USB an chance to connect to serial

	// init I2C and bme280-sensor
	Wire.begin(SDAPin, SCLPin);
	bme280.init();			// setup BME280


	TaskHandle_t hLightsBrunnen = lightsBrunnen.create();
	TaskHandle_t hLightsStufe = lightsStufe.create();
	TaskHandle_t hLightsRing = lightsRing.create();

//	resetController.setOnResetListener(onReset);
//	resetController.create();


	// create EventGroup for Network-Handling
  	hEventGroupNetwork = xEventGroupCreate();
  	assert(hEventGroupNetwork);

#ifdef USE_LOCAL_AP
	// create own access point
	Serial.print("Creating SoftAP ... ");
	WiFi.mode(WIFI_AP);

	IPAddress ipAddr;
	IPAddress gateway;
	IPAddress subnet;
	ipAddr.fromString(staticIP);
	gateway.fromString(staticGateway);
	subnet.fromString(staticSubnet);

	WiFi.softAPConfig(ipAddr, gateway, subnet);
	WiFi.softAP(wifi_ssid, wifi_password);
	Serial.println(WiFi.softAPIP());

	// hier die Einstellungen für hEventGroupNetwork setzen
	if ((xEventGroupGetBits(hEventGroupNetwork) & EG_NETWORK_INITIALIZED) == 0) {
		xEventGroupSetBits(hEventGroupNetwork, EG_NETWORK_INITIALIZED);
		//Serial.println(F("[WiFi] Set Event EG_NETWORK_INITIALIZED"));
	}
	// wenn connected Bit noch nicht gesetzt, dann hier setzen
	if ((xEventGroupGetBits(hEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
		xEventGroupSetBits(hEventGroupNetwork, EG_NETWORK_CONNECTED);
		Serial.println(F("[WiFi] Set Event EG_NETWORK_CONNECTED"));
	}
#else
/*
	// use home netwotk to connect
	//WiFi.disconnect();
	WiFi.mode(WIFI_STA);
	IPAddress ipAddr;
	IPAddress gateway;
	IPAddress subnet;
	IPAddress dns;

	ipAddr.fromString(staticIP);
	gateway.fromString(staticGateway);
	subnet.fromString(staticSubnet);
	dns.fromString(staticDNS);

	WiFi.config(ipAddr, gateway, subnet, dns);

  	WiFi.begin(ssid, password);
  	Serial.print("Connecting to WiFi ..");
  	while (WiFi.status() != WL_CONNECTED) {
	    Serial.print('.');
    	delay(1000);
  	}
  	Serial.println(WiFi.localIP());
	*/
	wifi.setStaticConfig(staticIP, staticGateway, staticSubnet, staticDNS);
	TaskHandle_t hWifiConnection = wifi.createConnection(&hEventGroupNetwork);
#endif

	
//	homeKit.setStaticConfig(staticIP, staticGateway, staticSubnet, staticDNS);
//	homeKit.setOnSetupHomeSpanListener(onSetupHKSwitches);
//	homeKit.setOnWifiEstablishedListener(onWifiEstablished);
//	TaskHandle_t hHomeKit = homeKit.createHomeKit();

	
	TaskHandle_t hNTP = ntp.createConnection(&hEventGroupNetwork);
	TaskHandle_t hFTP = ftp.createConnection(&hEventGroupNetwork);
	ftp.setCredentials(ftpUserName, ftpUserPassword);

/*
	TaskHandle_t hOTAConnection = ota.createConnection(&hEventGroupNetwork); 
*/	
	TaskHandle_t hTelnet = Telnet2.createConnection(&hEventGroupNetwork);
	Telnet2.setProjectAndVersion(const_cast<char*>(PROJECT_NAME), const_cast<char*>(SW_VERSION));
	Telnet2.setOnReadCommandListener(onTelnetReadCommand);

	//TaskHandle_t hMQTTConnection = mqtt.createConnection(&hEventGroupNetwork);
	//mqtt.setOnSubscribedTopicListener(mqttTopicCMDOpenGarage, OnSubscribedCMDOpenGarage);



	// Start FileSystem first

    // Initialize file system
    //MessageOutput.print("Initialize FS... ");		// from OpenDTU
    LOGGER.print("Initialize FS... ");
    if (!KSFileSystem.begin(false)) { // Do not format if mount failed
        LOGGER.print("failed... trying to format...");
        if (!KSFileSystem.begin(true)) {
            LOGGER.println("success");
        } else {
            LOGGER.println("failed");
        }
    } else {
        LOGGER.println("done");
    }
	// TODO format all space in KSFileSystem for the first time
    //KSFileSystem.format();
   	listDir(KSFileSystem, "/", 3);



	//deleteDataFromMemory();		// Just for initialization of memory. Its available via telnet "Erase"
	// load the last settings and initialize the hardware with this setting
	loadCurrentDataFromMemory(&brunnenTerrace);
	// initialize Hardware with settings
	char output[1024];
	createJSONMessage(output, sizeof(output));
	handleJSONMessage(output, sizeof(output));
	bSettingsChanged = false;



	// initialize WebSocket
  	ws.onEvent(onWsEvent);
  	server.addHandler(&ws);

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
   		LOGGER.printf("HTTP_GET: %s\n", request->url().c_str());
//		request->send(KSFileSystem, "/index.html", "text/html");
		request->send(KSFileSystem, "/index.html", "text/html", false, processor);
	});

	// handle the bootstrap-Files
	server.on("/src/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
   		LOGGER.printf("HTTP_GET: %s\n", request->url().c_str());
		request->send(KSFileSystem, "/src/bootstrap.bundle.min.js", "text/javascript");
	});
	server.on("/src/jquery-3.5.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
   		LOGGER.printf("HTTP_GET: %s\n", request->url().c_str());
		request->send(KSFileSystem, "/src/jquery-3.5.1.min.js", "text/javascript");
	});
	server.on("/src/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
   		LOGGER.printf("HTTP_GET: %s\n", request->url().c_str());
		request->send(KSFileSystem, "/src/bootstrap.min.css", "text/css");
	});

	server.onNotFound(notFound);

  	LOGGER.println("Starting WebServer on Port 80");
	server.begin();			// Start server

	
	wifi.waitForInit();		// TODO: Evtl. mit Zeit versehen oder komlett löschen
  	LOGGER.println("WiFi connection established");

	// da wir ein Problem mit dem Watchdoc und AsyncTCP haben, wird hier der Watchdog voh Hand gesetzt und in der loop gefüttert
	rtc_wdt_protect_off();    // Turns off the automatic wdt service
	rtc_wdt_enable();         // Turn it on manually
	rtc_wdt_set_time(RTC_WDT_STAGE0, 20000);  // Define how long you desire to let dog wait.
}



void loop() {
	// Test-Functions

	// read analog value for test
	//int potiValue = analogRead(SpeedControlPotiPin);			// we read with 12 Bit (0..4096)
	//int mapValue = map(potiValue, 0, 4096, 0, 255);		// we use 8 Bit
	
	//int mapValue = potiValue;   // we use 12 Bit PWM out

	// set the velocity on MOSFET-Module
	//setFontainHeight(mapValue);

	//lightsBrunnen.setBrightness((uint8_t)mapValue);
	//lightsBrunnen.setLightEffect(LightEffect::LERainbow);
	//lightsBrunnen.setLightEffect(LightEffect::LERainbowCycle);

	//lightsStufe.setBrightness((uint8_t)mapValue);
	//lightsStufe.setLightEffect(LightEffect::LEBlue);
	//lightsStufe.setLightEffect(LightEffect::LELastColor);

	//lightsRing.setBrightness((uint8_t)mapValue);
	//lightsRing.setLightEffect(LightEffect::LEGreen);
	//lightsRing.setLightEffect(LightEffect::LELastColor);


	// read temperature every 60 sec
	BEGIN_CYCLIC(ReadTemperature, READ_TEMPERATURE_INTERVALL)
		temperature = bme280.readTemperature();
		humidity = bme280.readHumidity();
		LOGGER.printf("Temperature: %.02f°C, Humidity: %.01f%%\n", temperature, humidity);

		// if temperature is to high, switch LEDs off
		if (temperature > 70.0) {
			LOGGER.printf("Warning: Temperature to high: %.02f°C, Humidity: %.01f%%\n", temperature, humidity);
			lightsBrunnen.setLightsOff();
			lightsStufe.setLightsOff();
			lightsRing.setLightsOff();
			bTemperatureToHigh = true;
		}
		// if temperature was to high and is now lower again, switch LEDs on
		if (bTemperatureToHigh && (temperature < 50.0)) {
			bTemperatureToHigh = false;

			// setze die Helligkeit und den Effect auf die vor dem ausschalten eingestellten Werte
			lightsBrunnen.setLightsOn();
			lightsStufe.setLightsOn();
			lightsRing.setLightsOn();
		}
	END_CYCLIC()


	// save last settings in preferences. Check all 5 minutes
 	BEGIN_CYCLIC(SavePreferences, SAVE_SETTINGS_INTERVALL)
		if (bSettingsChanged) {
			if (saveCurrentDataToMemory(&brunnenTerrace)) {
				bSettingsChanged = false;
			} else {
				LOGGER.println("Error in writing settings to preferences!");
			}
		}
	END_CYCLIC()

/*
	static unsigned long lastSavePreferencesMillis = 0;
	if (millis() - lastSavePreferencesMillis > SAVE_SETTINGS_INTERVALL) {
		if (bSettingsChanged) {
			if (saveCurrentDataToMemory(&brunnenTerrace)) {
				bSettingsChanged = false;
				lastSavePreferencesMillis = millis();
			} else {
				LOGGER.println("Error in writing settings do preferences!");
			}
		} else {
			lastSavePreferencesMillis = millis();
		}
	}
*/

	// check WebSocket Cleanup every 5sec
	BEGIN_CYCLIC(CleanUpClients, 5000)
		ws.cleanupClients();
	END_CYCLIC()


	rtc_wdt_feed();			// feed Watchdog
	vTaskDelay(pdMS_TO_TICKS(100));
}



void OnSubscribedCMDChangeVolume(char* topic, byte* payload, unsigned int length) {
//    Serial.print(F("OnSubscribedCMD ["));
//    Serial.print(topic);
//    Serial.print(F("] "));
//    for (int i = 0; i < length; i++) {
//        Serial.print((char)payload[i]);
//    }
//    Serial.println();

/*
	if (strcasecmp(topic, mqttTopicCMDOpenGarage) == 0) {
		// its this topic
		// interpret as string. append /0. TODO: Was ist, wenn der Buffer nicht reicht???
		payload[length] = '\0';
		if (strcasecmp((const char*)payload, "on") == 0) {
			// open the door
			LOGGER.println("cmd: open");
			moveGarageDoor = true;
//			mqtt.publish(topic, "off");		// Kommando zurücksetzen -> Absturz. Evtl falscher Thread?
		}
	}
*/
}

