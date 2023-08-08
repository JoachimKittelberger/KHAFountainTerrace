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
 * MIT License
 *
 * Copyright (c) 2023 Joachim Kittelberger - KibeSoft, www.kibesoft.de
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _GLOBALDEFINITIONS_H
#define _GLOBALDEFINITIONS_H
// evtl sinnvolle DSL APIs

#include <Arduino.h>
#include "KSESPFramework.h"


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