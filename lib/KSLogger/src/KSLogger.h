#ifndef _KSLOGGER_H
#define _KSLOGGER_H

#include <Arduino.h>

#if !defined(USE_TELNET1)
    #define USE_TELNET2
#endif

#if defined(USE_TELNET2)
    #include "KSTelnetServer2.h"
#else
    #include "KSTelnetServer.h"
#endif


// use Serial as Standard for Logging
#if !defined(LOG_SERIAL) && !defined(LOG_TELNET) && !defined(LOG_SERIAL_AND_TELNET)
    #define LOG_SERIAL
#endif

#if defined(LOG_SERIAL)
    #define LOGGER  Serial
#elif defined(LOG_TELNET)
    #if defined(USE_TELNET2)
        #define LOGGER  Telnet2
    #else
        #define LOGGER  Telnet
    #endif
#elif defined(LOG_SERIAL_AND_TELNET)
    #define LOGGER  SerialAndTelnet
#endif


class KSSerialAndTelnet : public Stream {

    public:
#if defined(USE_TELNET2)
        KSSerialAndTelnet(KSTelnetServer2* telnet = NULL) {
            _telnet = telnet;
        };
#else
        KSSerialAndTelnet(KSTelnetServer* telnet = NULL) {
            _telnet = telnet;
        };
#endif
        // Stream implementation
        int read();
        int available();
        int peek();


        // Print implementation
        virtual size_t write(uint8_t val);
        virtual size_t write(const uint8_t *buf, size_t size);
        using Print::write; // pull in write(str) and write(buf, size) from Print
        virtual void flush();

    private:
#if defined(USE_TELNET2)
       KSTelnetServer2* _telnet = NULL;
#else
        KSTelnetServer* _telnet = NULL;
#endif

};


extern KSSerialAndTelnet SerialAndTelnet;




#endif  // #define _KSLOGGER_H