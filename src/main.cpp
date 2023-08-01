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
 * 
 * @author Joachim Kittelberger <jkittelberger@kibesoft.de>
 * @date 01.07.2022
 * @version 1.00
 *
 * @todo
 *   + store last settings in preferences
 *   + set default values to other values
 *   + Change Partition-Table with CSV (more size and OTA)
 *   + Use KSOTA
 *   + put KSFileSystem in a class
 *   + put Fontain and IO functions in separat Files/classes
 *
 *   - Put lib in KSESPFramework
 *   - Use KSMQTTConnection
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
#include "KSLogger.h"
//#define LOGGER Serial		// TODO
//#define LOG_LEVEL LOG_LEVEL_TRACE
//#define LOG_LEVEL LOG_LEVEL_DEBUG


#define MQTT_MAX_SUBSCRIPTIONS 3

// just for Test use local access point and not home network
//#define USE_LOCAL_AP


#include <Arduino.h>
#include <Wire.h>

#include "AppInfo.h"
#include "AppCredentials.h"			// global App Credentials: must be copied from AppCredentials.h.tpl
#include "KSCredentials.h"			// global KSLibrariesCredentials: must bei copied from KSCredentials.h.tpl

#include "KSUtilities.h"

#include "JSONMessage.h"

#include "KSFileSystem.h"
KSFileSystemClass filesystem(LittleFS);

#include "KSTelnetServer2.h"
#include "Telnet.h"


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


#include "Fontain.h"
FontainClass fontainObj;


#ifdef ESP32
	#include <WiFi.h>
	// next 2 lines are because auf WDT-Problems in async_tcp
	#define CONFIG_ASYNC_TCP_RUNNING_CORE 1 // running on core1
	#define CONFIG_ASYNC_TCP_USE_WDT 0		// disable WDT for async_tcp because of WDT-Problems with async_tcp on core1
	#include <AsyncTCP.h>
#elif defined(ESP8266)
	//#include <ESP8266WiFi.h>
	//#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#include "KSEventGroupNetwork.h"
EventGroupHandle_t hEventGroupNetwork = NULL;

#include "KSWiFiConnection.h"
KSWiFiConnection wifi;

#include "KSNTPClient.h"
KSNTPClient ntp;

#include "KSFTPServer.h"
KSFTPServer ftp(LittleFS);


#include "KSOTA.h"
KSOTA ota(PROJECT_NAME, NULL, &server);				// Enable OTA


//KSMQTTConnection mqtt;
//KSResetController resetController(2*24*60*60);		// reset the device every 2d
//KSHomeKit homeKit(PROJECT_NAME, SW_VERSION);  // Projectname wird ignoriert. Wird von homeSpan Service DisplayName genommen

#include "GlobalDefinitions.h"
BrunnenData brunnenTerrace;


#include "Settings.h"
#define SAVE_SETTINGS_INTERVALL 5*60*1000		// save last settings all 5 min if something has changed
Settings settings(&brunnenTerrace);



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



// callback function
void onReset() {
  // is called before Reset of ESP32
  Serial.println("Reseting ESP32 ...");
  Serial.flush();
  vTaskDelay(pdMS_TO_TICKS(100));
}



// wird aufgerufen, wenn OTA in progress ist
void onOTAProgress(unsigned int progress, unsigned int total) {
    static unsigned int lastPercentage = 0;
	unsigned int percentage = progress / (total / 100);

	// show only if new value
	if (percentage != lastPercentage) {
		LOGGER.printf("OnOTAProgress %d%%\n", percentage);
		lastPercentage = percentage;
	}
}




// damit der Watchdog bei langen http-Aufrufen nicht zuschlägt, manuell auf höhere Zeit setzen
#include "soc/rtc_wdt.h"
// TODO in setup()
//rtc_wdt_protect_off();    // Turns off the automatic wdt service
//rtc_wdt_enable();         // Turn it on manually
//rtc_wdt_set_time(RTC_WDT_STAGE0, 20000);  // Define how long you desire to let dog wait.
// TODO in loop()
//rtc_wdt_feed(); // feed the Watchdog

// other WDT-Functions
//	disableCore0WDT();
//	disableCore1WDT();
//	enableCore0WDT();
//	enableCore0WDT();


void setup(){
	// init own MOSFET-Modul
	fontainObj.init();
	fontainObj.setFontainOff();
	brunnenTerrace.fontain.isFontainOn = false;

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


	ota.setProjectInfo(PROJECT_NAME, SW_VERSION);
	ota.setCredentials(updateUserName, updateUserPassword);			// TODO: Store the credentials in global credential File
	ota.setOnProgressListener(onOTAProgress);
	TaskHandle_t hOTAConnection = ota.createConnection(&hEventGroupNetwork); 
	
	TaskHandle_t hTelnet = Telnet2.createConnection(&hEventGroupNetwork);
	Telnet2.setProjectAndVersion(const_cast<char*>(PROJECT_NAME), const_cast<char*>(SW_VERSION));
	Telnet2.setOnReadCommandListener(onTelnetReadCommand);

	//TaskHandle_t hMQTTConnection = mqtt.createConnection(&hEventGroupNetwork);
	//mqtt.setOnSubscribedTopicListener(mqttTopicCMDOpenGarage, OnSubscribedCMDOpenGarage);

	
    // Initialize file system
	filesystem.init();
	filesystem.listDir("/", 3);


	//deleteSettingsFromMemory();		// Just for initialization of memory. Its available via telnet "Erase"
	// load the last settings and initialize the hardware with this setting
	settings.loadCurrentSettingsFromMemory();
	// initialize Hardware with settings
	char output[1024];
	createJSONMessage(output, sizeof(output));
	handleJSONMessage(output, sizeof(output));
	settings.changed(false);



	// initialize WebSocket
  	ws.onEvent(onWsEvent);
  	server.addHandler(&ws);

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
   		LOGGER.printf("HTTP_GET: %s\n", request->url().c_str());
//		request->send(KSFileSystem, "/index.html", "text/html");
//		request->send(KSFileSystem, "/index.html", "text/html", false, processor);
		request->send(filesystem.getKSFileSystemRef(), "/index.html", "text/html", false, processor);
	});

	// handle the bootstrap-Files
	server.on("/src/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
   		LOGGER.printf("HTTP_GET: %s\n", request->url().c_str());
//		request->send(KSFileSystem, "/src/bootstrap.bundle.min.js", "text/javascript");
		request->send(filesystem.getKSFileSystemRef(), "/src/bootstrap.bundle.min.js", "text/javascript");
	});
	server.on("/src/jquery-3.5.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
   		LOGGER.printf("HTTP_GET: %s\n", request->url().c_str());
//		request->send(KSFileSystem, "/src/jquery-3.5.1.min.js", "text/javascript");
		request->send(filesystem.getKSFileSystemRef(), "/src/jquery-3.5.1.min.js", "text/javascript");
	});
	server.on("/src/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
   		LOGGER.printf("HTTP_GET: %s\n", request->url().c_str());
//		request->send(KSFileSystem, "/src/bootstrap.min.css", "text/css");
		request->send(filesystem.getKSFileSystemRef(), "/src/bootstrap.min.css", "text/css");
	});


	server.on("/credentials.js", HTTP_GET, [](AsyncWebServerRequest *request){
   		LOGGER.printf("HTTP_GET: %s\n", request->url().c_str());
//		request->send(KSFileSystem, "/credentials.js", "text/javascript");
		request->send(filesystem.getKSFileSystemRef(), "/credentials.js", "text/javascript");
	});


	server.onNotFound(notFound);

  	LOGGER.println("Starting WebServer on Port 80");
	server.begin();			// Start server

	
	wifi.waitForInit();		// TODO: Evtl. mit Zeit versehen oder komlett löschen
  	LOGGER.println("WiFi connection established");

/*
	// da wir ein Problem mit dem Watchdoc und AsyncTCP auf cpu1 haben, wird hier der Watchdog voh Hand gesetzt und in der loop gefüttert
	rtc_wdt_protect_off();    // Turns off the automatic wdt service
	rtc_wdt_enable();         // Turn it on manually
	rtc_wdt_set_time(RTC_WDT_STAGE0, 20000);  // Define how long you desire to let dog wait.

//	disableCore0WDT();
//	disableCore1WDT();
//	enableCore0WDT();
//	enableCore0WDT();
*/
}



void loop() {
	// Test-Functions

	// read analog value for test
	//int potiValue = analogRead(SpeedControlPotiPin);			// we read with 12 Bit (0..4096)
	//int mapValue = map(potiValue, 0, 4096, 0, 255);		// we use 8 Bit
	
	//int mapValue = potiValue;   // we use 12 Bit PWM out

	// set the velocity on MOSFET-Module
	//fontainObj.setFontainHeight(mapValue);

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
		if (settings.hasChanged()) {
			if (settings.saveCurrentSettingsToMemory()) {
			} else {
				LOGGER.println("Error in writing settings to preferences!");
			}
		}
	END_CYCLIC()

/*
	static unsigned long lastSavePreferencesMillis = 0;
	if (millis() - lastSavePreferencesMillis > SAVE_SETTINGS_INTERVALL) {
		if (settings.hasChanged()) {
			if (settings.saveCurrentSettingsToMemory()) {
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

/*
	rtc_wdt_feed();			// feed Watchdog
*/
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

