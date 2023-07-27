/**
 * @file KSShelly.h
 * 
 * @brief controll shelly
 * 
 * @details 
 * 
 * @see
 * 
 * @author Joachim Kittelberger <jkittelberger@kibesoft.de>
 * @date 30.04.2022
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

#ifndef _KSSHELLY_H
#define _KSSHELLY_H


#include "Arduino.h"


class KSShelly {
    public:
        KSShelly(const char* shellyName) {
            _shellyName = shellyName;
        }
        KSShelly(String shellyName) {
            _shellyName = shellyName;
        }
        virtual ~KSShelly() {}

        bool setState(bool bOn = true, int id = 0);
        bool hasLastError() { return bLastError; }


    protected:
        String httpGETRequest(const char* requestURL);

    private:
        String _shellyName = "";
        bool bLastError = false;
};





#endif  // #define _KSSHELLY_H