/**
 * @file KSOTA.cpp
 * 
 * @brief implementation for using KSOTA
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
#include "KSOTA.h"

#include "KSEventGroupNetwork.h"
#include "KSResetController.h"
#include "KSDeepSleep.h"
#include "KSLogger.h"



KSOTA::KSOTA(const char *hostname, KSLiquidCrystal_I2C* pLCD, AsyncWebServer* pServer)
    : _esp32fota("esp32-fota-http", 1, false)
        , _cb_start(NULL)
        , _cb_end(NULL)
        , _cb_error(NULL)
        , _cb_progress(NULL)
{
    _pLCD = pLCD;
    _pServer = pServer;
    begin();
    
    if (hostname) {
        _projectName = hostname;
    }
}


KSOTA::~KSOTA() {
}


// Callbacks for user
KSOTA& KSOTA::setOnStartListener(ArduinoOTAClass::THandlerFunction fn) {
    _cb_start = fn;
    return *this;
}

KSOTA& KSOTA::setOnEndListener(ArduinoOTAClass::THandlerFunction fn) {
    _cb_end = fn;
    return *this;
}

KSOTA& KSOTA::setOnProgressListener(ArduinoOTAClass::THandlerFunction_Progress fn) {
    _cb_progress = fn;
    return *this;
}

KSOTA& KSOTA::setOnErrorListener(ArduinoOTAClass::THandlerFunction_Error fn) {
    _cb_error = fn;
    return *this;
}





TaskHandle_t KSOTA::createConnection(EventGroupHandle_t *phEventGroupNetwork) {

	_phEventGroupNetwork = phEventGroupNetwork;

	int coreID = xPortGetCoreID();
    //Serial.print(F("CoreID: "));
	//Serial.println(coreID);
	
	UBaseType_t setupPriority = uxTaskPriorityGet(NULL);
	//Serial.print(F("setup: priority = "));
	//Serial.println(setupPriority);

	xTaskCreatePinnedToCore(
    	[](void* context){ static_cast<KSOTA*>(context)->tKSOTA(); },
		"tKSOTA",
		8192,
		this,
		setupPriority,
		&_htKSOTA,
		coreID
		);

	return _htKSOTA;
}



void KSOTA::tKSOTA()
{
//    begin();

    // Warte auf Wifi-Verbindung und Initialisierung:
    // Wenn Connection-Bit noch nicht gesetzt, dann das erste Mal warten.
    if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
        if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
            //Serial.println(F("[ota] Wating for Event EG_NETWORK_CONNECTED"));
            EventBits_t eventGroupValue;
            eventGroupValue = xEventGroupWaitBits(*_phEventGroupNetwork, (EG_NETWORK_INITIALIZED | EG_NETWORK_CONNECTED), pdFALSE, pdTRUE, portMAX_DELAY);
            //Serial.println(F("[ota] Event EG_NETWORK_CONNECTED set"));
        }
    }
            
    // erst hier machen, da sonst Absturz. Vermutlich wegen Wifi noch nicht initialisiert.
    setupOTA();
	
	// main loop ota
    for (;;) {


        // Warte auf Wifi-Verbindung und Initialisierung:
        // Wenn Connection-Bit noch nicht gesetzt, dann das erste Mal warten.
        if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
            if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
                //Serial.println(F("[ota] Wating for Event EG_NETWORK_CONNECTED"));
                EventBits_t eventGroupValue;
                eventGroupValue = xEventGroupWaitBits(*_phEventGroupNetwork, (EG_NETWORK_INITIALIZED | EG_NETWORK_CONNECTED), pdFALSE, pdTRUE, portMAX_DELAY);
                //Serial.println(F("[ota] Event EG_NETWORK_CONNECTED set"));
            }
        }

	    ArduinoOTA.handle();

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}







void KSOTA::setupOTA() {
	// Handle ArduinoOTA
#ifdef ESP32
	ArduinoOTA.setPort(3232);		// ESP32: Standardport ist 3233. Kann aber abgeändert werden zur Sicherheit
#elif defined(ESP8266)
	ArduinoOTA.setPort(8266);		// ESP8266: Standardport ist 8266. Kann aber abgeändert werden zur Sicherheit
#endif
	//ArduinoOTA.setHostname("TestRelais");

    // if authentication required
    if (_authRequired && !_password.isEmpty()) {
        ArduinoOTA.setPassword(_password.c_str());
    }

    if (_projectName.isEmpty()) {
        LOGGER.println("WARNING: KSOTA::SetupOTA: No ProjectName defined! Please define one before starting OTA-Tasks");
    } else {
   	    ArduinoOTA.setHostname(_projectName.c_str());
    }

	ArduinoOTA.onStart([&]() {
        _onStart();
    });
	ArduinoOTA.onEnd([&]() {
        _onEnd();
    });
	ArduinoOTA.onProgress([&](unsigned int progress, unsigned int total) {
        _onProgress(progress, total);
    });
	ArduinoOTA.onError([&](ota_error_t error) {
        _onError(error);
    });

	ArduinoOTA.begin();
}





void KSOTA::_onStart() {   // do something before OTA Update
    if (_pLCD) {
	    if (_pLCD->_pcsI2C) _pLCD->_pcsI2C->EnterCriticalSection();
        _pLCD->clear();
        _pLCD->setCursor(0, 0);
        _pLCD->print("OTA-Update");
        _pLCD->setCursor(0, 1);
        _pLCD->print("Please Wait   0%");
    }
    Serial.printf("[ota]: Starting OTA-Update for %s. Current version is %s\n", _projectName.c_str(), _swVersion.c_str());
    //Serial.println("[ota]: onStart");
    // call handler function if exist
    if (_cb_start) {
        _cb_start();
    }
    if (_pDeepSleepHandler) {
        _pDeepSleepHandler->setOTAActive(true);
    }
    
}


void KSOTA::_onEnd() {                         // do something after OTA Update
    if (_pLCD) {
        _pLCD->clear();
        _pLCD->setCursor(0, 0);
        _pLCD->print("OTA-Update");
        _pLCD->setCursor(0, 1);
        _pLCD->print("End");
	    if (_pLCD->_pcsI2C) _pLCD->_pcsI2C->LeaveCriticalSection();
    }
    Serial.printf("[ota]: Successfull ended OTA-Update for %s. Old version was %s\n", _projectName, _swVersion);
    //Serial.println("[ota]: onEnd");
    // call handler function if exist
    if (_cb_end) {
        _cb_end();
    }
    if (_pDeepSleepHandler) {
        _pDeepSleepHandler->setOTAActive(false);
    }
}


void KSOTA::_onProgress(unsigned int progress, unsigned int total) {                         // do something before OTA Update
    unsigned int percentage = progress / (total / 100);
    if (_pLCD) {
        _pLCD->setCursor(12, 1);
        _pLCD->printf("%3u%%", percentage);
    }

    static unsigned int lastPercentage = 0;
    if (lastPercentage != percentage)  {
        Serial.printf("[ota]: %3u%%\n", percentage);
        lastPercentage = percentage;
    }

    // call handler function if exist
    if (_cb_progress) {
        _cb_progress(progress, total);
    }
}


void KSOTA::_onError(ota_error_t error) {
    Serial.printf("[ota]: Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
   	
 	if (_pLCD) {
        if (_pLCD->_pcsI2C) _pLCD->_pcsI2C->LeaveCriticalSection();
    }
    // call handler function if exist
    if (_cb_error) {
        _cb_error(error);
    }
}


/*
_cb_error(OTA_AUTH_ERROR OTA_BEGIN_ERROR OTA_CONNECT_ERROR OTA_RECEIVE_ERROR OTA_END_ERROR);
*/

void KSOTA::begin() {

    if (_pServer) {

        // send the MAC-Adress and Type of Controller as JSON to OTA-Page
        _pServer->on("/update/identity", HTTP_GET, [&](AsyncWebServerRequest *request) {
            if (_authRequired){
                if (!request->authenticate(_username.c_str(), _password.c_str())){
                    return request->requestAuthentication();
                }
            }
            #if defined(ESP8266)
                request->send(200, "application/json", "{\"id\": \"" + _id + "\", \"hardware\": \"ESP8266\", \"version\": \""+ _swVersion + "\", \"project\": \""+ _projectName + "\"}");
            #elif defined(ESP32)
                request->send(200, "application/json", "{\"id\": \""+ _id + "\", \"hardware\": \"ESP32\", \"version\": \""+ _swVersion + "\", \"project\": \""+ _projectName + "\"}");
            #endif
        });

        _pServer->on("/update", HTTP_GET, [&](AsyncWebServerRequest *request) {
            if (_authRequired){
                if (!request->authenticate(_username.c_str(), _password.c_str())){
                    return request->requestAuthentication();
                }
            }
            // Original Files from AsyncElegantOTA //https://github.com/ayushsharma82/ElegantOTA/tree/master/ui/src
//            AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ELEGANT_HTML, ELEGANT_HTML_SIZE);
//            response->addHeader("Content-Encoding", "gzip");
//            request->send(response);
 

            // benutzter Quellcode der Oberfläche von
            // https://www.mischianti.org/2021/11/11/esp8266-ota-update-with-web-browser-custom-web-interface-3/
		    request->send(KSFileSystem, "/update.html", "text/html");

        });

        _pServer->on("/update", HTTP_POST, [&](AsyncWebServerRequest *request) {
            if (_authRequired){
                if (!request->authenticate(_username.c_str(), _password.c_str())) {
                    return request->requestAuthentication();
                }
            }
            // the request handler is triggered after the upload has finished... 
            // create the response, add header, and send response
            AsyncWebServerResponse *response = request->beginResponse((Update.hasError())?500:200, "text/plain", (Update.hasError())?"FAIL":"OK");
            response->addHeader("Connection", "close");
            response->addHeader("Access-Control-Allow-Origin", "*");
            request->send(response);
			KSResetController::reset();
        }, [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            //Upload handler chunks in data
            if (_authRequired){
                if (!request->authenticate(_username.c_str(), _password.c_str())) {
                    return request->requestAuthentication();
                }
            }

            Serial.printf("Update: %s, index: %d, len: %d, final: %s\n", filename.c_str(), index, len, final ? "Yes" : "No");

            // start OTA
            if (!index) {
                if (!request->hasParam("MD5", true)) {
                    _cb_error(OTA_AUTH_ERROR);
                    return request->send(400, "text/plain", "MD5 parameter missing");
                }

                if (!Update.setMD5(request->getParam("MD5", true)->value().c_str())) {
                    _cb_error(OTA_AUTH_ERROR);
                    return request->send(400, "text/plain", "MD5 parameter invalid");
                }

            #if defined(ESP8266)
                int cmd = (filename == "filesystem") ? U_FS : U_FLASH;
                Update.runAsync(true);
                size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
                uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                if (!Update.begin((cmd == U_FS)?fsSize:maxSketchSpace, cmd)) { // Start with max available size
            #elif defined(ESP32)
                int cmd = (filename == "filesystem") ? U_SPIFFS : U_FLASH;
                if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) { // Start with max available size
            #endif
                    Update.printError(Serial);
                    _cb_error(OTA_BEGIN_ERROR);
                    return request->send(400, "text/plain", "OTA could not begin");
                }
                _onStart();
            }

            // Write chunked data to the free sketch space
            if (len) {
                if (Update.write(data, len) != len) {
                    _cb_error(OTA_BEGIN_ERROR);
                    return request->send(400, "text/plain", "OTA could not begin");
                }
            }
            
            // we have the last packet
            if (final) { // if the final flag is set then this is the last frame of data
                if (!Update.end(true)) { //true to set the size to the current progress
                    Update.printError(Serial);
                    _cb_error(OTA_END_ERROR);
                    return request->send(400, "text/plain", "Could not end OTA");
                }
                _onEnd();
            } else {
                return;
            }
        });


        // handle other needed files
        // icon for update page
        _pServer->on("/firmware-icon.png", HTTP_GET, [&](AsyncWebServerRequest *request) {
//            if (_authRequired){
//                if (!request->authenticate(_username.c_str(), _password.c_str())){
//                    return request->requestAuthentication();
//                }
//            }
		    request->send(KSFileSystem, "/firmware-icon.png", "image/png");
        });

        // handle the spark-md5.min.js
        _pServer->on("/spark-md5.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
//            if (_authRequired){
//                if (!request->authenticate(_username.c_str(), _password.c_str())){
//                    return request->requestAuthentication();
//                }
//            }
            request->send(KSFileSystem, "/spark-md5.min.js", "text/javascript");
        });





    }
}



// return the MAC-Address as unique id
String KSOTA::getID() {
    String id = "";
#if defined(ESP8266)
    id = String(ESP.getChipId());
#elif defined(ESP32)
    id = String((uint32_t)ESP.getEfuseMac(), HEX);
#endif
    id.toUpperCase();
    return id;
}





void KSOTA::forceHttpUpdate(String filename) {

 	String pathname = "http://192.168.30.220/KSAutoUpdate/binaries/" + filename;

    _esp32fota.setProgressCb(_cb_progress);
    _esp32fota.forceUpdate(pathname.c_str(), false); // check signature: true
}



