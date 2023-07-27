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