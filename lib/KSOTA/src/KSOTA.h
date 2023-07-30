/**
 * @file KSOTA.h
 * 
 * @brief Header file for using KSOTA
 * 
 * @details 
 * 
 * @see
 *   - https://github.com/chrisjoyce911/esp32FOTA.git @ ^0.2.7
 *   - https://github.com/iakop/LiquidCrystal_I2C_ESP32.git
 * 
 * @author Joachim Kittelberger <jkittelberger@kibesoft.de>
 * @date 20.06.2022
 * @version 1.00
 *
 * @todo
 *   - Wenn Passwort zur Laufzeit geändert wird, dies dann auch anpassen, damit es gleich verwendet werden kann
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

#ifndef _KSOTA_H
#define _KSOTA_H

#include <ArduinoOTA.h>                             // http://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html

#include "KSFileSystem.h"           // INFO: muss vor #include <esp32fota.h> kommen, damit dort das richtige FileSystem verwendet wird
// TODO for LittleFS: we have to change the order of detection of FileSysetem in esp32FOTA.hpp. First LittleFS and then SPIFFS
#include <esp32fota.h>

#include "KSLiquidCrystal_I2C.h"

#include "Update.h"
#include "ESPAsyncWebServer.h"


class KSDeepSleep;

class KSOTA {
    public:
        KSOTA(const char *hostname = NULL, KSLiquidCrystal_I2C* pLCD = NULL, AsyncWebServer* pServer = NULL);
        ~KSOTA();

        TaskHandle_t createConnection(EventGroupHandle_t *phEventGroupNetwork = NULL);

        //This callback will be called when OTA connection has begun
        KSOTA& setOnStartListener(ArduinoOTAClass::THandlerFunction fn);

        //This callback will be called when OTA has finished
        KSOTA& setOnEndListener(ArduinoOTAClass::THandlerFunction fn);

        //This callback will be called when OTA encountered Error
        KSOTA& setOnErrorListener(ArduinoOTAClass::THandlerFunction_Error fn);

        //This callback will be called when OTA is receiving data
        KSOTA& setOnProgressListener(ArduinoOTAClass::THandlerFunction_Progress fn);


        void setCredentials(const char* username, const char* password) {
            _username = username;
            _password = password;
            if (strlen(username) > 0) {
                _authRequired = true;
            } else {
                _authRequired = false;
                _username = "";
                _password = "";
            }
        }


        void setProjectInfo(const char* pProjectName, const char* pSWVersion) {
            if (pProjectName)
                _projectName = pProjectName;
            if (pSWVersion)
                _swVersion = pSWVersion;
        }


        void setID(const char* id) {
            _id = id;
        }
        void begin();

        void forceHttpUpdate(String filename);

        // disable DeepSleep during OTA-Update
        void setDeepSleepHandler(KSDeepSleep* pDeepSleepHandler) { _pDeepSleepHandler = pDeepSleepHandler; }

    private:
        void tKSOTA();
        void setupOTA();

        TaskHandle_t _htKSOTA;
        EventGroupHandle_t *_phEventGroupNetwork = NULL;


        void _onStart();                          // do something before OTA Update
        void _onEnd();                            // do something after OTA Update
        void _onProgress(unsigned int progress, unsigned int total);
        void _onError(ota_error_t error);

        KSLiquidCrystal_I2C* _pLCD = NULL;


        ArduinoOTAClass::THandlerFunction _cb_start;
        ArduinoOTAClass::THandlerFunction _cb_end;
        ArduinoOTAClass::THandlerFunction_Error _cb_error;
        ArduinoOTAClass::THandlerFunction_Progress _cb_progress;


        AsyncWebServer* _pServer = NULL;

        String getID();


        String _id = getID();
        String _username = "";
        String _password = "";

        String _projectName = "";
        String _swVersion = "0.0.0";
        bool _authRequired = false;

        esp32FOTA _esp32fota;
        KSDeepSleep* _pDeepSleepHandler = NULL;
};



#endif  // #define _KSOTA_H