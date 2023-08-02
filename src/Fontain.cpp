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


