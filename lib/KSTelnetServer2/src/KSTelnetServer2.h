/**
 * @file KSTelnetServer2.h
 * 
 * @brief Header file for KSTelnetServer2 class
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


#ifndef _KSTELNETSERVER2_H
#define _KSTELNETSERVER2_H


#include "KSESPTelnet.h"


class KSTelnetServer2 : public Print {
    public:
        KSTelnetServer2(uint16_t port = 23);
        ~KSTelnetServer2();

        TaskHandle_t createConnection(EventGroupHandle_t *phEventGroupNetwork = NULL);
 		
        void setDefaultCommandHandler(bool handleDefault) { _handleDefaultCommands = handleDefault; }
        void setOnReadCommandListener(void (*callback)(char* szCommand)) { onReadCommandListener = callback; }
        void setOnReadSetCommandListener(void (*callback)(char* szParameter, char* szValue)) { onReadSetCommandListener = callback; }
    
        bool shouldSendCyclicData() { return _sendCyclicData; }
        void setProjectAndVersion(char* pProject, char* pVersion) { _pPROJECT_NAME = pProject; _pSW_VERSION = pVersion; }
        
        static void onInputReceived(String str);
        static void onConnect(String ip);
        static void onConnectionAttempt(String ip);
        static void onReconnect(String ip);
        static void onDisconnect(String ip);

        static KSTelnetServer2* obj;     // for singleton. Used for callbacks

     	virtual size_t write(uint8_t b) override {
            return write(&b, 1);
        }
        virtual size_t write(const uint8_t *buf, size_t size) override {
            size_t ret = 0;
            ret = _telnet.write(buf, size);
            return ret;
        }

        using Print::write;

        void flush() {
            _telnet.flush();
        }


    protected:
       void (*onReadCommandListener)(char* szCommand);
       void (*onReadSetCommandListener)(char* szParameter, char* szValue);

       void setupCallbacks();

    private:
        void tKSTelnetServer();

        TaskHandle_t _htKSTelnetServer;
        EventGroupHandle_t *_phEventGroupNetwork = NULL;

        bool _handleDefaultCommands = true;
        bool _sendCyclicData = false;

        uint16_t _port = 23;
        KSESPTelnet _telnet;

        static char* _pSW_VERSION;
        static char* _pPROJECT_NAME;
};


extern KSTelnetServer2 Telnet2;



#endif  // #define _KSTELNETSERVER2_H