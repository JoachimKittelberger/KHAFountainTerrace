/**
 * @file Telnet.cpp
 *
 * @brief implementation file for using telnet commands
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
#include "Telnet.h"

#include "GlobalDefinitions.h"

#include "KSESPFrameworkSettings.h"

#include "Fontain.h"
#include "Settings.h"
#include "JSONMessage.h"
#include "AppInfo.h"


extern float temperature;
extern float humidity;

extern BrunnenData brunnenTerrace;
extern FontainClass fontainObj;
extern Settings settings;



void onTelnetReadCommand(char* szCommand) {

	if (strcasecmp(szCommand, "Info") == 0) {
		LOGGER.printf("Project: %s Version: %s\n", PROJECT_NAME, SW_VERSION);
		LOGGER.printf("Temperature: %.02f°C, Humidity: %.01f%%\n", temperature, humidity);

		char * buffer = listESPStateJSON();
		LOGGER.println(buffer);
		free(buffer);
	}
	else if (strcasecmp(szCommand, "Erase") == 0) {
		LOGGER.println("Erase all Data");
		settings.deleteSettingsFromMemory();
//		homeKit.EraseFlashData();
	}
	else if (strcasecmp(szCommand, "Save") == 0) {
		LOGGER.print("Save Data ...");
		if (settings.saveCurrentSettingsToMemory()) {
			LOGGER.println(" Succeeded!");
		} else {
			LOGGER.println(" Error in Saving!");
		}
	}
	else if (strcasecmp(szCommand, "Fontain On") == 0) {
		LOGGER.println("Set Fontain On");
		fontainObj.setFontainOn();
		brunnenTerrace.fontain.isFontainOn = true;
		settings.changed();
	}
	else if (strcasecmp(szCommand, "Fontain Off") == 0) {
		LOGGER.println("Set Fontain Off");
		fontainObj.setFontainOff();
		brunnenTerrace.fontain.isFontainOn = false;
		settings.changed();
	}
	else if (strStartsWith(szCommand, "Fontain Height")) {		// use: "Fontain Height=150"
		// get String after "Fontain Height"
		int height = -1;
		char* pChar = strrchr(szCommand, '=');
		if (pChar) {
			if (strlen(pChar) > 2)
				pChar++;		// go behind the '=' sign
			height = atoi(pChar);
		}
		if (height > -1) {
			fontainObj.setFontainHeight(height);
			brunnenTerrace.fontain.height = height;
			settings.changed();
		}
		LOGGER.printf("Set Fontain Height to %d\n", height);
	}

	else if (strcasecmp(szCommand, "SolarMode") == 0) {
		LOGGER.println("Switch to Solar Mode");
		fontainObj.switchToSolarMode();
		brunnenTerrace.fontain.isSolarOn = true;
		settings.changed();
	}
	else if (strcasecmp(szCommand, "PowerMode") == 0) {
		LOGGER.println("Switch to Power Mode");
		fontainObj.switchToPowerMode();
		brunnenTerrace.fontain.isSolarOn = false;
		settings.changed();
	}
	else {
		LOGGER.printf("[telnet] Unkown command: %s\n", szCommand);
	}
}


