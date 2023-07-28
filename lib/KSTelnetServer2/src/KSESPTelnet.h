/**
 * @file KSESPTelnet.h
 * 
 * @brief Header file for KSESPTelnet class
 * 
 * @details 
 * 
 * @see
 *   - lennarthennigs/ESP Telnet @ ^2.1.2
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

#pragma once

#include <ESPTelnet.h>



/* override ESPTelnet and add support for printf-Funcitonality */

class KSESPTelnet : public ESPTelnet {

  public:
    KSESPTelnet() : ESPTelnet() {}


    size_t write(const uint8_t *buf, size_t size) {
      size_t ret = 0;
      ret = client.write(buf, size);
      return ret;
    }
    void flush() {
      client.flush();
    }

};

  // << operator
  //template<class T> inline KSESPTelnet &operator <<(KSESPTelnet &obj, T arg) { obj.print(arg); return obj; } 

