
#include "KSRTC.h"

#include <sys/time.h>
#include "KSUtilities.h"


// lokale Zeitzone definieren
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"           // Timezone for Berlin
#define TIMEZONE_OFFSET -3600                           // -1h


KSRTC::KSRTC(KSCriticalSection* pcsI2C) {
    _pcsI2C = pcsI2C;
}


KSRTC::~KSRTC() {
}




bool KSRTC::init(bool bSetCompileTimeIfLost) {
    bool bRetVal = true;
	
    if (_pcsI2C) _pcsI2C->EnterCriticalSection();
    boolean foundRTC = _rtc.begin();
    if (_pcsI2C) _pcsI2C->LeaveCriticalSection();
    
    if (!foundRTC) {
        Serial.print("[KSRTC] Could not find RTC.");

        if (bSetCompileTimeIfLost) {
            Serial.print(" Let's set the time to Compile-Time of sketch!");

            // set local time to date & time this sketch was compiled
            DateTime dtnow = DateTime(F(__DATE__), F(__TIME__));
            setLocalProcessorDateTime(dtnow);
        }
        Serial.println();
 
        _bHasHWRTC = false;
        bRetVal = false;

    } else {
        
        _bHasHWRTC = true;      // We have a hardware RTC
        

        if (lostPower()) {
            Serial.print("[KSRTC] RTC lost power!");
            if (bSetCompileTimeIfLost) {
                Serial.print(" Let's set the time to Compile-Time of sketch!");
                // When time needs to be set on a new device, or after a power loss, the
                // following line sets the RTC to the date & time this sketch was compiled
                adjust(DateTime(F(__DATE__), F(__TIME__)));
                // This line sets the RTC with an explicit date & time, for example to set
                // January 21, 2014 at 3am you would call:
                // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
            }
            Serial.println();
        }
        if (_pcsI2C) _pcsI2C->EnterCriticalSection();
        _rtc.disable32K();
        if (_pcsI2C) _pcsI2C->LeaveCriticalSection();
    }

//    Serial.print("[KSRTC] RTC time init: ");
//    Serial.println(String(getLocalDateTimeFormated()).c_str());

    return bRetVal;
}



// muss in setup() immer zuerst gesetzt werden (nach jedem reset neu)
void KSRTC::initTimeZoneGermany() {
	unsetenv("TZ");
	setenv("TZ", TIMEZONE_GERMANY, 1);             // Zeitzone  muss nach dem reset neu eingestellt werden
	tzset();
}



bool KSRTC::lostPower() {
    if (_pcsI2C) _pcsI2C->EnterCriticalSection();
    boolean bLostPower = _rtc.lostPower();
    if (_pcsI2C) _pcsI2C->LeaveCriticalSection();
    return bLostPower;
}



void KSRTC::adjust(tm* ptm) { 
    DateTime dt = DateTime(ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    adjust(dt);
}



void KSRTC::adjust(const DateTime &dt) { 
    // if we have not hardware-RTC, return without doing anything
    // TODO: write local time instead
    if (!_bHasHWRTC) {
    	Serial.println("Failed to adjust RTC. No RTC-Hardware found. Writing to localtime.");
        setLocalProcessorDateTime(dt);
        return;
    }

    if (_pcsI2C) _pcsI2C->EnterCriticalSection();
    _rtc.adjust(dt);
    if (_pcsI2C) _pcsI2C->LeaveCriticalSection();
}


float KSRTC::getTemperature() {
    float fRetVal;

    if (_pcsI2C) _pcsI2C->EnterCriticalSection();
    fRetVal = _rtc.getTemperature();
    if (_pcsI2C) _pcsI2C->LeaveCriticalSection();

    return fRetVal;
}





DateTime KSRTC::now() {

    // if we have not rtc, use local system time
    DateTime dt;

    if (!_bHasHWRTC) {
	    struct tm timeinfo;
		if (!getLocalTime(&timeinfo)) {
			Serial.println("Failed to obtain time");
		}
        dt = DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

/*        
        time_t now = time(0);
	    tm* ptm = localtime(&now);
        dt = DateTime(ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
*/
        return dt;
    }

    if (_pcsI2C) _pcsI2C->EnterCriticalSection();
    dt = _rtc.now();
    if (_pcsI2C) _pcsI2C->LeaveCriticalSection();

    return dt;
}



// gibt die lokale Prozessorzeit zurück, die vorher von der RTC gesetzt wurde
// Zeit kann mit getLocalTime(&tm) gelesen werden
void KSRTC::getLocalProcessorDateTime(tm* pTimeInfo) {
	getLocalTime(pTimeInfo);
	Serial.println(pTimeInfo, "Datum: %d.%m.%y  Zeit: %H:%M:%S"); // Zeit Datum Print Ausgabe formatieren
}



// setzt die Systemzeit aus der RTC
void KSRTC::setLocalProcessorDateTimeFromRTC() {
    DateTime dtnow = this->now();
    setLocalProcessorDateTime(dtnow);
}



// setzt die Systemzeit
// dabei wird tm.tm_isdst verwendet, um Sommerzeit automatisch in mktime zu berechnen
// der CET-Offset muss aber von Hand gemacht werden
void KSRTC::setLocalProcessorDateTime(const DateTime &dt) {

    struct tm tm = { 0 };
    tm.tm_year = dt.year() - 1900;   // Set date
    tm.tm_mon = dt.month() - 1;
    tm.tm_mday = dt.day();
    tm.tm_hour = dt.hour();      // Set time
    tm.tm_min = dt.minute();
    tm.tm_sec = dt.second();
    tm.tm_isdst = isSummerTime(&tm) ? 1 : 0;        // this subs 1h in mktime or timeInfo2time_t

    time_t t = timeInfo2time_t(&tm);
    t = t + TIMEZONE_OFFSET;          // we are at CET-1, sub 1h
    //Serial.printf("Setting time: %s\n", asctime(&tm));

    struct timeval tvnow = { 0, 0 };
    tvnow.tv_sec = t;
    settimeofday(&tvnow, NULL);

    //Serial.printf("setLocalProcessorDateTimeFromRTC RTC: %s\n", getLocalDateTimeFormated());
    //struct tm tminfo = { 0 };
    //getLocalProcessorDateTime(&tminfo);

//	setenv("TZ", TIMEZONE_GERMANY, 1);             // Zeitzone  muss nach dem reset neu eingestellt werden
//	tzset();
}   




const char* KSRTC::getLocalDateTimeFormated() {
    DateTime dt = now();
    snprintf(_szFormatedDateTime, sizeof(_szFormatedDateTime), PSTR("%02d.%02d.%04d %02d:%02d:%02d"),
            dt.day(), dt.month(), dt.year(), dt.hour(), dt.minute(), dt.second());

    return _szFormatedDateTime;
}



const char* KSRTC::getLocalDateFormated() {
    DateTime dt = now();
    snprintf(_szFormatedDate, sizeof(_szFormatedDate), PSTR("%02d.%02d.%04d"),
            dt.day(), dt.month(), dt.year());

    return _szFormatedDate;
}


const char* KSRTC::getLocalDateFormatedReverse() {
    DateTime dt = now();
    snprintf(_szFormatedDate, sizeof(_szFormatedDate), PSTR("%04d%02d%02d"),
            dt.year(), dt.month(), dt.day());

    return _szFormatedDate;
}


const char* KSRTC::getLocalTimeFormated() {
    DateTime dt = now();
    snprintf(_szFormatedTime, sizeof(_szFormatedTime), PSTR("%02d:%02d:%02d"),
            dt.hour(), dt.minute(), dt.second());

    return _szFormatedTime;
}



deviceUptime KSRTC::getDeviceUptime() {

	unsigned long currentmillis = millis();

	deviceUptime uptime;
	uptime.secs  = (long)((currentmillis / 1000) % 60);
	uptime.mins  = (long)((currentmillis / 60000) % 60);
	uptime.hours = (long)((currentmillis / 3600000) % 24);
	uptime.days  = (long)((currentmillis / 86400000) % 10);

	return uptime;
}

String KSRTC::getDeviceUptimeString() {

	deviceUptime uptime = getDeviceUptime();

	return	String(uptime.days) + " days, " +
	        String(uptime.hours) + " hours, " +
	        String(uptime.mins) + " mins, " +
	        String(uptime.secs) + " secs";
}


String KSRTC::getDateTimeISO8601Formated(int year, int month, int day, int hour, int minute, int second) {
    char timeBuffer[ISO8601_LEN];
//    snprintf(timeBuffer, sizeof(timeBuffer), PSTR("%04d-%02d-%02dT%02d:%02d:%02d"),
//            year, month, day, hour, minute, second);
    snprintf(timeBuffer, sizeof(timeBuffer), PSTR("%04d-%02d-%02dT%02d:%02d:%02dZ"),
            year, month, day, hour, minute, second);

    return String(timeBuffer);
}

String KSRTC::getDateTimeISO8601Formated(tm timeinfo) {
	return getDateTimeISO8601Formated(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}


void KSRTC::getDateTimeFromISO8601(char iso8601[ISO8601_LEN], int* pYear, int* pMonth, int* pDay, int* pHour, int* pMinute, int* pSecond) {
    if (pYear && pMonth && pDay && pHour && pMinute && pSecond) {
//        sscanf(iso8601, PSTR("%04d-%02d-%02dT%02d:%02d:%02d"), pYear, pMonth, pDay, pHour, pMinute, pSecond);
        sscanf(iso8601, PSTR("%04d-%02d-%02dT%02d:%02d:%02dZ"), pYear, pMonth, pDay, pHour, pMinute, pSecond);
    }
}



String KSRTC::getLocalDateTimeISO8601Formated() {
    DateTime dt = now();
    return getDateTimeISO8601Formated(dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
}



char* KSRTC::datetime2ISO8601(char iso8601[ISO8601_LEN], DateTime& dt) {
//    snprintf(iso8601, ISO8601_LEN, PSTR("%04d-%02d-%02dT%02d:%02d:%02d"),
    snprintf(iso8601, ISO8601_LEN, PSTR("%04d-%02d-%02dT%02d:%02d:%02dZ"),
            dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());

    return iso8601;
}



char* KSRTC::timeval2ISO8601(char iso8601[ISO8601_LEN], uint8_t precision, const struct timeval *tv) {
    struct tm tm;
    if (!gmtime_r(&tv->tv_sec, &tm))
        return (char*)memcpy(iso8601, "Error: Year overflow", sizeof("Error: Year overflow"));

    tm.tm_year %= 10 * 1000;
    char *frac = iso8601 + strftime(iso8601, sizeof("1970-01-01T23:59:59."), "%Y-%m-%dT%H:%M:%SZ", &tm);

    if (precision) {
        unsigned long usecs = tv->tv_usec;
        for (int i = precision; i < 6; i++)
			usecs /= 10;
        char *spaces = frac + sprintf(frac - 1, ".%-*luZ", precision, usecs) - 3;
        if (spaces > frac)
			while (*spaces == ' ')
				*spaces-- = '0';
    }

    return iso8601;
}






bool KSRTC::isDateTimeValid(tm* pTimeinfo) {
	if (!pTimeinfo)
		return false;
	if (pTimeinfo->tm_year < 2000)
		return false;
	if (pTimeinfo->tm_year > 2100)
		return false;
	return true;
}




/*  Daten mit Grenzwerten zum testen der Funktionen:
    29.12.2014 = ist ein Montag und faellt bereits in die 1. Woche des naechsten Jahres
    31.12.2015 = in dem Jahr gab es eine 53. Woche
    31.12.2016 = Tag des Jahres wird mit 366 korrekt berechnet (Schaltjahr)
    01.01.2017 = ist ein Sonntag und faellt noch in die 52. Woche des Vorjahres
*/
/*
  uint16_t year = 2014;
  uint8_t month = 12;
  uint8_t day = 29;
  const char WeekDays[][3] = {"Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"}; // Abkuerzungen der Wochentage
*/
const int t_Table[] PROGMEM = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

// ***** Den Wochentag nach ISO 8601 (1 = Mo, 2 = Di, 3 = Mi, 4 = Do, 5 = Fr, 6 = Sa, 7 = So) berechnen ****
uint8_t GetWeekday(uint16_t y, uint8_t m, uint8_t d) {
    y -= m < 3;
    uint8_t wd = (y + y / 4 - y / 100 + y / 400 + pgm_read_word(t_Table + m - 1) + d) % 7;
    return (wd == 0 ? 7 : wd);
}

// ***** Die Wochennummer nach ISO 8601 berechnen ****
uint8_t GetWeekNumber(uint16_t y, uint8_t m, uint8_t d) {
    bool LeapYear;
    uint16_t doy = GetDayOfYear(y, m, d);  // Anzahl der Tage im Jahr ermitteln
    uint8_t wd = GetWeekday(y, m, d);      // Wochentag ermitteln
    uint8_t wnr = (doy - wd + 7) / 7;      // die Wochennummer berechnen
    switch (wnr) {
        case 0:                              // wenn die Wochennummer Null ergibt, dann liegt der Tag am Anfang des Jahres (1. Sonderfall)
            wd = GetWeekday(y - 1, 12, 31);    // den letzten Wochentag aus dem Vorjahr ermitteln
            LeapYear = IsLeapYear(y - 1);      // ermitteln, ob es sich beim Vorjahr um ein Schaltjahr handelt
        break;                             // und nach dem Switch weitermachen...
        case 52:                             // wenn die Wochennummer 52 ergibt, dann liegt der Tag am Ende des Jahres (2. Sonderfall)
            wd = GetWeekday(y, 12, 31);        // den letzten Wochentag aus diesem Jahr ermitteln
            LeapYear = IsLeapYear(y);          // ermitteln, ob es sich bei diesem Jahr um ein Schaltjahr handelt
        break;                             // und nach dem Switch weitermachen...
        	default:                             // in den anderen Faellen kann die Funktion
        return wnr;                        // hier verlassen und die Wochennummer zurueckgegeben werden
    }
    if (wd < 4) {                          // wenn der 31.12. vor dem Donnerstag liegt, dann...
        wnr = 1;                             // ist das die erste Woche des Jahres
    } else {                               // anderenfalls muss ermittelt werden, ob es eine 53. Woche gibt (3. Sonderfall)
        // wenn der letzte Wochentag auf einen Donnerstag oder,
        // in einem Schaltjahr, auf einen Donnerstag oder Freitag fällt,
        // dann ist das die 53. Woche, ansonsten die 52. Woche.
        wnr = ((wd == 4) || (LeapYear && wd == 5)) ? 53 : 52;
    }
    return wnr;
}

const uint16_t mdays_Table[] PROGMEM = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

// ***** die Anzahl der Tage (Tag des Jahres) berechnen ****
uint16_t GetDayOfYear(uint16_t y, uint8_t m, uint8_t d) {
  	return d + pgm_read_word(mdays_Table + m - 1) + (m >= 2 && IsLeapYear(y));
}

// ***** Testen, ob das Jahr ein Schaltjahr ist ****
bool IsLeapYear(uint16_t y) {
  	return  !(y % 4) && ((y % 100) || !(y % 400)); // Schaltjahrberechnung (true = Schaltjahr, false = kein Schaltjahr)
}

