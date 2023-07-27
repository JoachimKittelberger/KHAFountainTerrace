#ifndef _KSRTC_H
#define _KSRTC_H


#include <RTClib.h>
#include "KSCriticalSection.h"


#define ISO8601_LEN (sizeof("1970-01-01T23:59:59.123456Z"))
#define TIMEZONE_GERMANY "CET-1CEST,M3.5.0,M10.5.0/3"           // Timezone for Berlin


struct deviceUptime {
    long days;
    long hours;
    long mins;
    long secs;
};


/***** Den Wochentag nach ISO 8601 (1 = Mo, 2 = Di, 3 = Mi, 4 = Do, 5 = Fr, 6 = Sa, 7 = So) berechnen *****/
uint8_t GetWeekday(uint16_t y, uint8_t m, uint8_t d);

/***** Die Wochennummer nach ISO 8601 berechnen *****/
uint8_t GetWeekNumber(uint16_t y, uint8_t m, uint8_t d);

/***** die Anzahl der Tage (Tag des Jahres) berechnen *****/
uint16_t GetDayOfYear(uint16_t y, uint8_t m, uint8_t d);

/***** Testen, ob das Jahr ein Schaltjahr ist *****/
bool IsLeapYear(uint16_t y);




class KSRTC {
    public:
        KSRTC(KSCriticalSection* pcsI2C);
        ~KSRTC();

        bool init(bool bSetCompileTimeIfLost = true);
        static void initTimeZoneGermany();

        void adjust(const DateTime &dt);
        void adjust(tm* ptm);
        DateTime now();
        float getTemperature();
        bool lostPower();

        void getLocalProcessorDateTime(tm* pTimeInfo);
        void setLocalProcessorDateTimeFromRTC();
        void setLocalProcessorDateTime(const DateTime &dt);   
        
        const char* getLocalDateTimeFormated();
        const char* getLocalDateFormated();
        const char* getLocalDateFormatedReverse();
        const char* getLocalTimeFormated();
        String getLocalDateTimeISO8601Formated();

        static String getDateTimeISO8601Formated(int year, int month, int day, int hour, int minute, int second);
        static String getDateTimeISO8601Formated(tm timeinfo);
        static void getDateTimeFromISO8601(char iso8601[ISO8601_LEN], int* pYear, int* pMonth, int* pDay, int* pHour, int* pMinute, int* pSecond);
        static char* datetime2ISO8601(char iso8601[ISO8601_LEN], DateTime& dt);
        static char* timeval2ISO8601(char iso8601[ISO8601_LEN], uint8_t precision, const struct timeval *tv);

        static bool isDateTimeValid(tm* pTimeinfo);
        bool hasHardwareRTC() { return _bHasHWRTC; }

        deviceUptime getDeviceUptime();
        String getDeviceUptimeString();


    private:
        KSCriticalSection* _pcsI2C = NULL;
        
        RTC_DS3231 _rtc;
        bool _bHasHWRTC = false;

        char _szFormatedDateTime[ISO8601_LEN] = "12.07.1969 13:21:25";
        char _szFormatedDate[ISO8601_LEN] = "12.07.1969";
        char _szFormatedTime[ISO8601_LEN] = "13:21:25";
};


#endif  // #define _KSRTC_H