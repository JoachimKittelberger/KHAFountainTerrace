/**
 * @file KSFTPServer.cpp
 * 
 * @brief implementation for KSFTPServer class
 * 
 * @details 
 * 
 * @author Joachim Kittelberger <jkittelberger@kibesoft.de>
 * @date 20.06.2022
 * @version 1.00
 *
 * @todo
 *
 * @bug
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

#include "KSFTPServer.h"
#include "KSEventGroupNetwork.h"

#include "KSFileSystem.h"

//extern const char ftpUserName[];
//extern const char ftpUserPassword[];


KSFTPServer::KSFTPServer() : ftpSrv(KSFileSystem) {
}


KSFTPServer::~KSFTPServer() {
}




TaskHandle_t KSFTPServer::createConnection(EventGroupHandle_t *phEventGroupNetwork) {

	_phEventGroupNetwork = phEventGroupNetwork;

	int coreID = xPortGetCoreID();
	//Serial.print(F("CoreID: "));
	//Serial.println(coreID);
	
	UBaseType_t setupPriority = uxTaskPriorityGet(NULL);
	//Serial.print(F("setup: priority = "));
	//Serial.println(setupPriority);

	xTaskCreatePinnedToCore(
        [](void* context){ static_cast<KSFTPServer*>(context)->tKSFTPServer(); },
		"tKSFTPServer",
		8192,
		this,
		setupPriority,
		&_htKSFTPServer,
		coreID
		);

	return _htKSFTPServer;
}




void KSFTPServer::tKSFTPServer()
{
    // Warte auf Wifi-Verbindung und Initialisierung:
    // Wenn Connection-Bit noch nicht gesetzt, dann das erste Mal warten.
    if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
        if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
            //Serial.println(F("[ftp] Wating for Event EG_NETWORK_CONNECTED"));
            EventBits_t eventGroupValue;
            eventGroupValue = xEventGroupWaitBits(*_phEventGroupNetwork, (EG_NETWORK_INITIALIZED | EG_NETWORK_CONNECTED), pdFALSE, pdTRUE, portMAX_DELAY);
            //Serial.println(F("[ftp] Event EG_NETWORK_CONNECTED set"));
        }
    }
            
    // erst hier machen, da sonst Absturz. Vermutlich wegen Wifi noch nicht initialisiert.
    //ftpSrv.begin(ftpUserName, ftpUserPassword);
    ftpSrv.begin(_pUsername, _pPassword);
	
	// main loop ftp
    for (;;) {

        // Warte auf Wifi-Verbindung und Initialisierung:
        // Wenn Connection-Bit noch nicht gesetzt, dann das erste Mal warten.
        if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
            if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
                //Serial.println(F("[ftp] Wating for Event EG_NETWORK_CONNECTED"));
                EventBits_t eventGroupValue;
                eventGroupValue = xEventGroupWaitBits(*_phEventGroupNetwork, (EG_NETWORK_INITIALIZED | EG_NETWORK_CONNECTED), pdFALSE, pdTRUE, portMAX_DELAY);
                //Serial.println(F("[ftp] Event EG_NETWORK_CONNECTED set"));
            }
        }

        ftpSrv.handleFTP();
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}



