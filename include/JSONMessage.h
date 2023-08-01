/**
 * @file JSONMessage.h
 *
 * @brief Header file for using JSONMessage
 * 
 * @details 
 *  Wird verwendet um Daten mit der WebSeite über WebSockets auszutauschen
 * @see
 *   - bblanchon/ArduinoJson @ ^6.21.3									; JSON
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
#ifndef _JSONMESSAGE_H
#define _JSONMESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>




void createJSONMessage(char* pOutput, size_t len);
void handleJSONMessage(const char* data, size_t len);

char * listESPStateJSON();




#endif          // #define _JSONMESSAGE_H