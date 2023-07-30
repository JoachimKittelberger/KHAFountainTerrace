/**
 * @file KSResetController.h
 * 
 * @brief Header file for using KSResetController
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
#pragma once

#include <ArduinoOTA.h>                             // http://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html


/* Example of Using:

// declaraion of object
KSResetController resetController(3*24*60*60);

// callback function
void onReset() {
  // is called before Reset of ESP32
  Serial.println("Reseting ESP32 ...");
  Serial.flush();
  vTaskDelay(pdMS_TO_TICKS(100));
}

// in setup()
  //resetController.setResetTime(3*24*60*60);
  resetController.setOnResetListener(onReset);
  resetController.create();

*/


// Reset the Controller after a defined period of time
// max unsigned long = 4,294,967,295 = 71582 min = 1193h = 49d

#define MAX_ULONG 4294967295
#define INIT_RESETTIME_IN_SEC (3 * 24 * 60 * 60)        // 3days


class KSResetController
{
    public:
        KSResetController(unsigned long resetTimeInSec = INIT_RESETTIME_IN_SEC);
        ~KSResetController();
  
        TaskHandle_t create();
        
        void setResetTime(unsigned long resetTimeInSec) {
            _resetTimeInMillis = resetTimeInSec * 1000;
        }
        static void reset();

        void setOnResetListener(void (*callback)()) { onResetListener = callback; }

    
    protected:
       void (*onResetListener)();

 
    private:
        void tKSResetController();
        TaskHandle_t _htKSResetController;

        unsigned long _resetTimeInMillis;
};
