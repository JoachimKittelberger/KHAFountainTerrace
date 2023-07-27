/**
 * @file KSWS2812B.h
 * 
 * @brief Header file for using WS2812B
 * 
 * @details 
 * 
 * 
 * @author Joachim Kittelberger <jkittelberger@kibesoft.de>
 * @date 20.06.2022
 * @version 1.00
 *
 * @todo
 * - Weitere effecte einbauen unter:
 *   - https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 *   - https://github.com/kitesurfer1404/WS2812FX
 *   - https://adrianotiger.github.io/Neopixel-Effect-Generator/
 *   - https://kno.wled.ge/advanced/custom-features/
 *
 * @bug bug description
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

#ifndef _KSWS2812B_H
#define _KSWS2812B_H



#include <Adafruit_NeoPixel.h>

// we need a critical secction
//#define DONT_INCLUDE_KSLLibraries_CPP 1         // we need this because we are not in KSLibraries-Path
//#define USE_KSCriticalSection 1
//#include "KSLibraries.h"

#include "KSCriticalSection.h"

// TODO: Evtl. die einzelnen Effekte in Klassen packen, die ihr Verhalten dann selber steuern können
// TODO: Enum und Struct umbenennen und bei get die Struktur zurückgeben


// if we add new effect we have also add them in
// const LEStruct lightEffects[LE_NUMBER_OF_EFFECTS]
// void KSWS2812B::handle()
enum LightEffect {
    LEBlack = 0,
    LEWhite,
    LERed,
    LEGreen,
    LEBlue,
    LELastColor,
    LERainbow,
    LERainbowCycle,


    LENumOfLightEffects
};




typedef struct {
    LightEffect leEffect;
    const char* pEffectName;
    const char* pEffectDescription;
    uint32_t defaultAnimationTime;
    // hier noch defaultspeed ... aufnehmen
} LEStruct;



extern const LEStruct lightEffects[LightEffect::LENumOfLightEffects];




class KSWS2812B
{
    public:
        KSWS2812B(int numPixel, int pin);
        ~KSWS2812B() {};

        TaskHandle_t create();

        void setBrightness(uint8_t brightness);
        uint8_t getBrigthness() { return _currentBrightness; }

        void setLightEffect(LightEffect effect);
        LightEffect getLightEffect() { return _currentLightEffect; }
        void updateLights() { _lightEffectChanged = true; }

        void setLightsOff();
        void setLightsOn();
        bool areLightsOn() { return !_lightsOff; }

        void setAnimationTimeMS(int timeMS) {
            if (timeMS >= _currentScheduleTimer) _currentAnimationTimeMS = timeMS;
            else _currentAnimationTimeMS = _currentScheduleTimer;
        }
        int getAnimationTimeMS() { return _currentAnimationTimeMS; }


        void setColor(uint8_t r, uint8_t g, uint8_t b);
        void setColor(uint32_t color);
        uint32_t getColor() { return _lastColor; }

    protected:
        bool init();            // could be used without create() (without separat thread). But must be make public in this case
        void handle();          // could be used without create() (without separat thread). But must be make public in this case

        void setAllPixelToColor(uint8_t r, uint8_t g, uint8_t b);
        void setAllPixelToColor(uint32_t color);
        void setRainbow(uint16_t firstPixel, uint16_t first_hue, int8_t reps, uint8_t saturation, uint8_t brightness, bool gammify); 


    private:
        volatile LightEffect _currentLightEffect = LightEffect::LEBlack;
        volatile uint32_t _lastColor = 0xFF0000;         // default is red

        volatile bool _lightEffectChanged = true;
        volatile uint8_t _currentBrightness = 128;
        int _currentAnimationCycle = 0;
        int _currentAnimationTimeMS = 200;      // Time for Animation-Change
        volatile bool _lightsOff = false;

        Adafruit_NeoPixel _neoPixel;

        void tKSWS2812B();
        TaskHandle_t _htKSWS2812B;
        volatile int _currentScheduleTimer = 20;         // default is 20ms for scheduling Animation-Task

        int _numPixel = 0;                          // Number of LEDs in stripe
        int _pin = 0;                               // Pin for WS2812B-Stripe Data

        KSCriticalSection csPixelsAccess;			// Critical Section for access to Jet32-API and synchronize calls
};






#endif  // #define _KSWS2812B_H