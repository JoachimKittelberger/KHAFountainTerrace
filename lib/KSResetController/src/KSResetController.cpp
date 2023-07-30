/**
 * @file KSResetController.cpp
 * 
 * @brief Implementation of KSResetController
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
#include "KSResetController.h"


KSResetController::KSResetController(unsigned long resetTimeInSec) {
	onResetListener = NULL;
	_resetTimeInMillis = resetTimeInSec * 1000;
	//create();
}


KSResetController::~KSResetController() {

}
  
  
TaskHandle_t KSResetController::create() {
	int coreID = xPortGetCoreID();
	UBaseType_t setupPriority = uxTaskPriorityGet(NULL);

	xTaskCreatePinnedToCore(
    	[](void* context){ static_cast<KSResetController*>(context)->tKSResetController(); },
		"tKSResetController",
		8192,
		this,
		setupPriority,
		&_htKSResetController,
		coreID
		);

	return _htKSResetController;
}
        
 


void KSResetController::tKSResetController() {
	Serial.println("created Task tKSResetController");
	for (;;) {
		unsigned long currentMillis = millis();
		if (currentMillis >= _resetTimeInMillis) {

			Serial.print("ResetController reset after ");
			Serial.print(currentMillis);
			Serial.println("ms. Restarting ESP ...");
			Serial.flush();
			
			if (onResetListener) {
				onResetListener();
			}
			reset();
		} else {
/*			Serial.print("Check Resetcontroller current ms: ");
			Serial.print(currentMillis);
			Serial.print(" Reset at: ");
			Serial.println(_resetTimeInMillis);
*/		}
		vTaskDelay(pdMS_TO_TICKS(1000));

	}	
}



void KSResetController::reset() {
	yield();
	vTaskDelay(pdMS_TO_TICKS(100));
	yield();
	ESP.restart();
}
