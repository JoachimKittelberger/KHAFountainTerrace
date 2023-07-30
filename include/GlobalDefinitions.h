/**
 * @file GlobalDefinitions.h
 * 
 * @brief Header file for global definitions for KHAFountainTerrace project
 * 
 * @details 
 * 
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
#ifndef _GLOBALDEFINITIONS_H
#define _GLOBALDEFINITIONS_H
// evtl sinnvolle DSL APIs

#include <Arduino.h>

#include "KSWS2812B.h"

typedef struct {
    bool isOn = false;              // LED on or off
    int brightness = 100;           // 0 .. 255
    uint32_t color;                 // RGB
    LightEffect lightEffect = LightEffect::LEBlack;
    int lightEffectSpeed = 100;        // Geschwindigkeit des Effects in ms (20 .. max)
} NeoPixelLED;



typedef struct {
    bool isSolarOn = true;         // Solar on or off
    bool isFontainOn = false;       // Fontain on or off
    int height = 0;      // Height of fontain 0 .. 255

} Fontain;


typedef struct {
    NeoPixelLED ledBrunnen;
    NeoPixelLED ledRing;
    NeoPixelLED ledStufe;
    Fontain fontain;
} BrunnenData;



/*
// JSON-Document für WebService und Kommunikation mit html-page und mqtt
{
    "ledbrunnen": {
        "mode":"on",
        "brightness":100,
        "colorRGB": "#0433ff",
        "lighteffect":"LEDBlack",       // hier eventuell mit Zahlen arbeiten oder Liste mit FLASHHELPER Array und Texten durchgehen und Index ermitteln
        "lespeed":100
    },
    "ledring": {
    ...
    },
    "ledstufe": {
    ...
    },
    
    "fontain": {
        "solar":"on",
        "mode":"on",
        "height":150
    }
}
*/





void initModeRelaisModul();
void switchToSolarMode(bool bSolarMode = true);
void switchToPowerMode();
bool isInSolarMode();

void initFontain();
void setFontainOn(bool bOn = true);
void setFontainOff();
bool isFontainOn();
void setFontainHeight(int height = 255);        // maximum hight = 255. Minimum Hight = 150? Eventuell hier in % arbeiten
int getFontainHeight();



/*

// homekit mit RGB-LED implementieren

// LEDs:
void setLEDOn(bool bOn = true);
void setLEDOff();

void setBrightness(int brightness);          // vermutlich nur von 0-255 evtl. hier in Prozent angeben
void setEffectMode(enum EffectModes mode);   // hier die verschiedenen Effekte angeben als Enum
*/

// LED-Ansteuerung in einen separaten Thread auslagern.
// Das ganze über MQTT ansteuerbar machen
// Das ganze über webserver ansteurbar machen. (Homepage)

// das ganze über Hue oder Homekit steuerbar machen









#endif          // #define _GLOBALDEFINITIONS_H