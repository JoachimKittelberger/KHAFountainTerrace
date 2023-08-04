/**
 * @file Shelly.cpp
 *
 * @brief implementation file for using shelly
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
#include "Shelly.h"

#include "KSESPFrameworkSettings.h"
#include "AppCredentials.h"




// switch of the power via the shelly in the device
void switchPowerOff() {
	// Lights Terrasse Shelly off
	
	KSShelly shelly(ShellyIPTerrasseLEDs);
	bool bLastState = shelly.setState(false, 0);
	if (shelly.hasLastError()) {
		LOGGER.printf("Error in Shelly set to false.\n");
	}
	LOGGER.printf("Shelly set to false. Last State was: %s\n", bLastState ? "true" : "false");
}

