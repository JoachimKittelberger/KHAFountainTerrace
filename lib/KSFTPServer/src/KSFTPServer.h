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