/**
 * @file Fontain.cpp
 *
 * @brief Implementation for Fontain class
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
#include "Fontain.h"


void FontainClass::init() {
	initModeRelaisModul();
	initFontain();
}



// ****************************************************************************
// Functions for handling mode relais
void FontainClass::initModeRelaisModul() {
	pinMode(ModeRelais1Pin, OUTPUT);
	pinMode(ModeRelais2Pin, OUTPUT);
	switchToSolarMode();			// default is solar mode
}


void FontainClass::switchToSolarMode(bool bSolarMode) {
    if (!bSolarMode) {
        switchToPowerMode();
	} else {
		digitalWrite(ModeRelais1Pin, true);		// ausschalten
		digitalWrite(ModeRelais2Pin, true);
    }
}


void FontainClass::switchToPowerMode() {
	digitalWrite(ModeRelais1Pin, false);	// einschalten
	digitalWrite(ModeRelais2Pin, false);
}


bool FontainClass::isInSolarMode() {
	if (digitalRead(ModeRelais1Pin) && digitalRead(ModeRelais2Pin))
		return true;		// we are in Solar mode
	return false;
}
// ****************************************************************************


// ****************************************************************************
// Functions for handling Power Mosfet fontain
void FontainClass::initFontain() {
	pinMode(MotorPWMPin, OUTPUT);
	digitalWrite(MotorPWMPin, false);
	ledcSetup(LEDC_CHANNEL_MOTORPWM, MOTORPWM_FREQ, LEDC_TIMER_8_BIT);
	ledcAttachPin(MotorPWMPin, LEDC_CHANNEL_MOTORPWM);
	_lastFontainHeight = 150;
}


void FontainClass::setFontainOn(bool bOn) {
	if (bOn) {
		ledcWrite(LEDC_CHANNEL_MOTORPWM, _lastFontainHeight);
	} else {
		setFontainOff();
	}
}


void FontainClass::setFontainOff() {
	ledcWrite(LEDC_CHANNEL_MOTORPWM, 0);
}


bool FontainClass::isFontainOn() {
	if (ledcRead(LEDC_CHANNEL_MOTORPWM) == 0) {
		return false;
	}
	return true;
}


void FontainClass::setFontainHeight(int height) {        // maximum hight = 255. Minimum Hight = 150? Eventuell hier in % arbeiten
	// set the velocity on MOSFET-Module
	ledcWrite(LEDC_CHANNEL_MOTORPWM, height);
	_lastFontainHeight = height;
}


int FontainClass::getFontainHeight() {
	return _lastFontainHeight;
}


