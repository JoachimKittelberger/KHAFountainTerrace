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
#include "Telnet.h"

#include "KSESPFramework.h"
#include "GlobalDefinitions.h"

#include "Fontain.h"
#include "Settings.h"
#include "JSONMessage.h"



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


