/**
 * @file JSONMessage.cpp
 *
 * @brief implementation file for using JSONMessage
 * 
 * @details 
 *  Wird verwendet um Daten mit der WebSeite über WebSockets auszutauschen
 * @see
 * 
 * @author Joachim Kittelberger <jkittelberger@kibesoft.de>
 * @date 20.06.2022
 * @version 1.00
 *
 * @todo
 *
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
#include "JSONMessage.h"
#include "GlobalDefinitions.h"
#include "KSLogger.h"
#include "KSWS2812B.h"
#include "Fontain.h"
#include "Settings.h"
#include "Shelly.h"		// we controll the power of the LEDs and Fontain with a shelly at 220V side


extern float temperature;
extern float humidity;

extern BrunnenData brunnenTerrace;

extern KSWS2812B lightsBrunnen;
extern KSWS2812B lightsStufe;
extern KSWS2812B lightsRing;

extern FontainClass fontainObj;
extern Settings settings;

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
					if (!fontainObj.isInSolarMode()) {
						fontainObj.switchToSolarMode();
						brunnenTerrace.fontain.isSolarOn = true;
						settings.changed();
					}
				} else if (strcmp(fontain_solar, "off") == 0) {
					if (fontainObj.isInSolarMode()) {
						fontainObj.switchToPowerMode();
						brunnenTerrace.fontain.isSolarOn = false;
						settings.changed();
					}
				}
			}

			if (fontain.containsKey("mode")) {
				const char* fontain_mode = fontain["mode"]; // "on"
				LOGGER.printf(" mode: %s", fontain_mode);
				if (strcmp(fontain_mode, "on") == 0) {
					fontainObj.setFontainOn();
					brunnenTerrace.fontain.isFontainOn = true;
					settings.changed();
				} else if (strcmp(fontain_mode, "off") == 0) {
					fontainObj.setFontainOff();
					brunnenTerrace.fontain.isFontainOn = false;
					settings.changed();
				}
			}

			if (fontain.containsKey("height")) {
				int fontain_height = fontain["height"]; // 150
				fontain_height = constrain(fontain_height, 0, 255);
				LOGGER.printf(" height: %d", fontain_height);
				fontainObj.setFontainHeight(fontain_height);
				brunnenTerrace.fontain.height = fontain_height;
				settings.changed();
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

		settings.changed();				// something has changed
	}
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
