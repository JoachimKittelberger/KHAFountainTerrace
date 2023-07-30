/**
 * @file Settings.h
 *
 * @brief Implementation of Settings class
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
#include "Settings.h"


Settings::Settings(BrunnenData* pBrunnen) : _pBrunnen(pBrunnen) {

}


bool Settings::saveCurrentSettingsToMemory() {
    if (!_pBrunnen)
        return false;

	KSPreferences prefs;
	if (prefs.begin(PREFKEY_FOUNTAIN_TERRACE, false)) {

		// names must be max 15 chars
		// save fontain data
		prefs.putBool("issolaron", _pBrunnen->fontain.isSolarOn);
		prefs.putBool("isfontainon", _pBrunnen->fontain.isFontainOn);
		prefs.putInt("height", _pBrunnen->fontain.height);
			
		// save ledBrunnen data
		prefs.putBool("brison", _pBrunnen->ledBrunnen.isOn);
		prefs.putInt("brbrightness", _pBrunnen->ledBrunnen.brightness);
		prefs.putUInt("brcolor", _pBrunnen->ledBrunnen.color);
		prefs.putInt("brlighteffect", _pBrunnen->ledBrunnen.lightEffect);
		prefs.putInt("brlespeed", _pBrunnen->ledBrunnen.lightEffectSpeed);
		
		// save ledRing data
		prefs.putBool("riison", _pBrunnen->ledRing.isOn);
		prefs.putInt("ribrightness", _pBrunnen->ledRing.brightness);
		prefs.putUInt("ricolor", _pBrunnen->ledRing.color);
		prefs.putInt("rilighteffect", _pBrunnen->ledRing.lightEffect);
		prefs.putInt("rilespeed", _pBrunnen->ledRing.lightEffectSpeed);

		// save ledStufe data
		prefs.putBool("stison", _pBrunnen->ledStufe.isOn);
		prefs.putInt("stbrightness", _pBrunnen->ledStufe.brightness);
		prefs.putUInt("stcolor", _pBrunnen->ledStufe.color);
		prefs.putInt("stlighteffect", _pBrunnen->ledStufe.lightEffect);
		prefs.putInt("stlespeed", _pBrunnen->ledStufe.lightEffectSpeed);

		_hasChanged = false;
		prefs.end();
		return true;
	}
	prefs.end();
	return false;
}


bool Settings::loadCurrentSettingsFromMemory() {
    if (!_pBrunnen)
        return false;

	KSPreferences prefs;
	if (prefs.begin(PREFKEY_FOUNTAIN_TERRACE, true)) {
		// names must be max 15 chars
		// load fontain data
		_pBrunnen->fontain.isSolarOn = prefs.getBool("issolaron", false);
		_pBrunnen->fontain.isFontainOn = prefs.getBool("isfontainon", false);
		_pBrunnen->fontain.height = prefs.getInt("height", 150);
		_pBrunnen->fontain.height = constrain(_pBrunnen->fontain.height, 0, 255);
		
		// load ledBrunnen data
		_pBrunnen->ledBrunnen.isOn = prefs.getBool("brison", false);
		_pBrunnen->ledBrunnen.brightness = prefs.getInt("brbrightness", 100);
		_pBrunnen->ledBrunnen.brightness = constrain(_pBrunnen->ledBrunnen.brightness, 0, 255);
		_pBrunnen->ledBrunnen.color = prefs.getUInt("brcolor", 0x0000FF);
		int le = prefs.getInt("brlighteffect", LightEffect::LEBlack);
		le = constrain(le, 0, LightEffect::LENumOfLightEffects - 1);
		_pBrunnen->ledBrunnen.lightEffect = (LightEffect)le;
		_pBrunnen->ledBrunnen.lightEffectSpeed = prefs.getInt("brlespeed", 100);
		_pBrunnen->ledBrunnen.lightEffectSpeed = constrain(_pBrunnen->ledBrunnen.lightEffectSpeed, 20, 2000);

		// load ledRing data
		_pBrunnen->ledRing.isOn = prefs.getBool("riison", false);
		_pBrunnen->ledRing.brightness = prefs.getInt("ribrightness", 100);
		_pBrunnen->ledRing.brightness = constrain(_pBrunnen->ledRing.brightness, 0, 255);
		_pBrunnen->ledRing.color = prefs.getUInt("ricolor", 0x0000FF);
		le = prefs.getInt("rilighteffect", LightEffect::LEBlack);
		le = constrain(le, 0, LightEffect::LENumOfLightEffects - 1);
		_pBrunnen->ledRing.lightEffect = (LightEffect)le;
		_pBrunnen->ledRing.lightEffectSpeed = prefs.getInt("rilespeed", 100);
		_pBrunnen->ledRing.lightEffectSpeed = constrain(_pBrunnen->ledRing.lightEffectSpeed, 20, 4000);

		// load ledStufe data
		_pBrunnen->ledStufe.isOn = prefs.getBool("stison", false);
		_pBrunnen->ledStufe.brightness = prefs.getInt("stbrightness", 100);
		_pBrunnen->ledStufe.brightness = constrain(_pBrunnen->ledStufe.brightness, 0, 255);
		_pBrunnen->ledStufe.color = prefs.getUInt("stcolor", 0x0000FF);
		le = prefs.getInt("stlighteffect", LightEffect::LEBlack);
		le = constrain(le, 0, LightEffect::LENumOfLightEffects - 1);
		_pBrunnen->ledStufe.lightEffect = (LightEffect)le;
		_pBrunnen->ledStufe.lightEffectSpeed = prefs.getInt("stlespeed", 100);
		_pBrunnen->ledStufe.lightEffectSpeed = constrain(_pBrunnen->ledStufe.lightEffectSpeed, 20, 2000);

		_hasChanged = false;
		prefs.end();
		return true;
	}
	prefs.end();
	return false;
}


void Settings::deleteSettingsFromMemory() {
	KSPreferences prefs;

	if (prefs.begin(PREFKEY_FOUNTAIN_TERRACE, false)) {
		prefs.clear();		// alle Keys unterhalb Namespace löschen

		// oder keys einzeln löschen
		// names must be max 15 chars
		// prefs.remove("issolaron");	// remove key from namespace
		_hasChanged = true;
	}
	
	prefs.end();
}


