/**
 * @file Fontain.h
 *
 * @brief Header file for using Fontain class
 * 
 * @details 
 * Behandelt die Funktion des Springbrunnens und stellt die Höhe usw ein
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
#ifndef _FONTAIN_H
#define _FONTAIN_H

#include <Arduino.h>


// ModeRelais for switching between solar and power mode
const int ModeRelais1Pin = 21;
const int ModeRelais2Pin = 22;


#include <driver/ledc.h>		// for PWM output ledc-functions
// Pins for controlling motor PWM with own MOSEFT-Driver-Modul
const int MotorPWMPin = 25;
#define MOTORPWM_FREQ		5000
#define LEDC_CHANNEL_MOTORPWM LEDC_CHANNEL_0




class FontainClass {
    public:
        FontainClass() {}
    
    public:
        void init();
        
        // handling Relaismodul
        void initModeRelaisModul();
        void switchToSolarMode(bool bSolarMode = true);
        void switchToPowerMode();
        bool isInSolarMode();

        // handling Fontain MOSFET
        void initFontain();
        void setFontainOn(bool bOn = true);
        void setFontainOff();
        bool isFontainOn();
        void setFontainHeight(int height = 255);        // maximum hight = 255. Minimum Hight = 150? Eventuell hier in % arbeiten
        int getFontainHeight();

    private:
        int _lastFontainHeight = 0;
};


#endif          // #define _SETTINGS_H