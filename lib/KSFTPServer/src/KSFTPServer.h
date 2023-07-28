/**
 * @file KSFTPServer.h
 * 
 * @brief Header file for KSFTPServer class
 * 
 * @details 
 * 
 * @see
 *   - https://github.com/dplasa/FTPClientServer.git
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

#ifndef _KSFTPSERVER_H
#define _KSFTPSERVER_H


#include <FTPServer.h>


class KSFTPServer {
    public:
        KSFTPServer();
        ~KSFTPServer();

        TaskHandle_t createConnection(EventGroupHandle_t *phEventGroupNetwork = NULL);
        void setCredentials(const char* pUsername, const char* pPassword) { _pUsername = const_cast<char*>(pUsername); _pPassword = const_cast<char*>(pPassword); }

    private:
        void tKSFTPServer();

        FTPServer ftpSrv;

        TaskHandle_t _htKSFTPServer;
        EventGroupHandle_t *_phEventGroupNetwork = NULL;

        char* _pUsername = NULL;
        char* _pPassword = NULL;

};



#endif  // #define _KSFTPSERVER_H