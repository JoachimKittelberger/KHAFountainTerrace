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