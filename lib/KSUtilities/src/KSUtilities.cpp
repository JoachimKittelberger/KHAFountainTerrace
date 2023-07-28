/**
 * @file KSUtilities.cpp
 * 
 * @brief implementation for KSUtilities functions
 * 
 * @details 
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

#include "KSUtilities.h"
#include <stdio.h>
#include <string.h>
#include <math.h>


// check, if an string starts with an given prefixstring
//#define STARTS_WITH(string_to_check, prefix) (strncmp(string_to_check, prefix, ((sizeof(prefix) / sizeof(prefix[0])) - 1)) ? 0:((sizeof(prefix) / sizeof(prefix[0])) - 1))
bool strStartsWith(const char *str, const char *prefix) {
  while (*prefix) {
    if (*prefix++ != *str++)
      return false;
  }
  return true;
}




// formats the output off __DATE__ to german format
// buff must be minimum [12]
void formatDATE2DE(char const *date, char *buff, size_t len) { 
    int month, day, year;
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(date, "%s %d %d", buff, &day, &year);
    month = (strstr(month_names, buff) - month_names) / 3 + 1;
    snprintf(buff, len, "%02d.%02d.%d", day, month, year);
}



void printSketchInfo(char const* currentProject) {
	Serial.printf("\n\nSketchname: %s\nBuild: %s\t\tIDE: %d.%d.%d\nESP-SDK: %s\n",
					(currentProject), (__TIMESTAMP__), ARDUINO / 10000, ARDUINO % 10000 / 100, ARDUINO % 100 / 10 ? ARDUINO % 100 : ARDUINO % 10, String(ESP.getSdkVersion()).c_str());
}




// CRC-Brechnung
uint8_t calculateCRCXOR(uint8_t* pBuffer, int count) {
    unsigned char temp = 0;
    for (int i = 0; i < count; i++) {
        temp ^= pBuffer[i];
    }
    
    temp &= 0xFF;
    return temp;
}





char getHexHigh(uint8_t value) {
    char cRet = (value >> 4) + 0x30;
    if (cRet > 0x39)
        cRet += 7;
    
    return cRet;
}



char getHexLow(uint8_t value) {
    char cRet = (value & 0x0f) + 0x30;
    if (cRet > 0x39)
        cRet += 7;
    
    return cRet;
}




//-----------------------------------------------------------------------------
///
/// Source code is copied from url https://www.hackster.io/erikuchama/day-of-the-week-calculator-cde704
///
/// @status  Draft
/// $Date:$
/// $Revision$
/// $LastChangedBy:$
//-----------------------------------------------------------------------------

/*
  Day of the Week calculator

  Accurate as far back as September 14, 1752.  
  In 1752, England was the last country to
  skip days (11 days) in order to sync up 
  calendars as the world finished shifting from 
  the Julian calendar to the Gregorian calendar.
  (Well...technically England was not the last 
  to change.  Russian calendars did not shift to 
  the Gregorian calendar until 1918, and Greece 
  shifted in 1923)
  
  Any date entered prior to this shift may 
  result in incorrect day of the week calculations

  Using formula:
  DoW value = (d + m + y + (y/4) + c)mod7

  where
  d = day
  m = month value (as per table below)
  y = two digit year
  c = century value (as per table below)

  - - - - - - - - - - - - - - - - - - - - 
 | Century |  Month        | DoW | Value |
 | - - - - | - - - - - - - | - - | - - - | 
 |    20   |  Jan/Oct      | Sat |   0   |
 |    19   |  May          | Sun |   1   |
 |    n/a  |  *Feb/Aug     | Mon |   2   |
 |    22   |  Feb/Mar/Nov  | Tue |   3   |
 |    n/a  |  Jun          | Wed |   4   |
 |    21   |  Sep/Dec      | Thu |   5   |
 |    n/a  |  *Jan/Apr/Jul | Fri |   6   |
  - - - - - - - - - - - - - - - - - - - -

 Note:  For Jan and Feb, if the year is a 
 leap year, use the *Jan or *Feb.  Otherwise 
 use regular Jan or Feb. 
 (see Leap Year note below)

 When all values are entered into formula
 (all values are absolute, so whatever you get
 for y/4, drop the decimal), divide by 7.  
 The remainder (0-6) goes back to the chart 
 to determine Day of the week.

 Leap Year Note:
 A leap year is determined true if:
 -1- The year is evenly divisible by 4, but not divisible by 100
 or
 -2- The year is evenly divisible by 400.

 Created by Eric Sitler
 30August2016

*/


//#include <math.h>


int GetLastWeekDayInMonth(int weekDay, int month, int year)
{
   int numOfDays = GetNumberOfDaysPerMonth(month, year);

   while (numOfDays > 0)
   {
      if (GetDayOfWeekForDate(numOfDays, month, year) == weekDay)
         return numOfDays;
      numOfDays--;
   }
   return numOfDays;
}

int GetDayOfWeekForDate(int day, int month, int year)
{
    //int m;          // Month Entry
    //int d;          // Day Entry
    int yy;         // Last 2 digits of the year (ie 2016 would be 16)
    //int yyyy;       // Year Entry
    int c;          // Century (ie 2016 would be 20)
    int mTable;     // Month value based on calculation table
    int SummedDate; // Add values combined in prep for Mod7 calc
    int DoW;        // Day of the week value (0-6)
    int leap;       // Leap Year or not
    int cTable;     // Century value based on calculation table
    
    // Leap Year Calculation
    if ((fmod(year, 4) == 0 && fmod(year, 100) != 0) || (fmod(year, 400) == 0))
        leap = 1;
    else 
        leap = 0;

    // Limit results to year 1900-2299 (to save memory)
    while (year > 2299) {
        year = year - 400;
    }
    while (year < 1900) {
        year = year + 400;
    }

    // Calculating century
    c = year / 100;

    // Calculating two digit year
    yy = fmod(year, 100);

    // Century value based on Table
    if (c == 19)
        cTable = 1;
    if (c == 20)
        cTable = 0;
    if (c == 21)
        cTable = 5;
    if (c == 22)
        cTable = 3;

    // Jan and Feb calculations affected by leap years
    if (month == 1) {
        if (leap == 1) {
            mTable = 6;
        }
        else {
            mTable = 0;
        }
    }
    if (month == 2) {
        if (leap == 1) {
            mTable = 2;
        }
        else {
            mTable = 3;
        }
    }
    
    // Other months not affected and have set values
    if (month == 10)
        mTable = 0;
    if (month == 8)
        mTable = 2;
    if (month == 3 || month == 11)
        mTable = 3;
    if (month == 4 || month == 7)
        mTable = 6;
    if (month == 5)
        mTable = 1;
    if (month == 6)
        mTable = 4;
    if (month == 9 || month == 12)
        mTable = 5;

    // Enter the data into the formula
    SummedDate = day + mTable + yy + (yy / 4) + cTable;
    
    // Find remainder
    DoW = fmod(SummedDate, 7);  

    return DoW;
}


int GetNumberOfDaysPerMonth(int month, int year)
{
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8|| month == 10|| month == 12)
        return 31;
    else if (month == 2 || month == 4 || month == 6 || month == 9 || month == 11)
        return 30;
    else {
        if ((fmod(year, 4) == 0 && fmod(year, 100) != 0) || (fmod(year, 400) == 0))
            return 29;
        else 
            return 28;
    }
}







bool isSummerTime(struct tm* pTimeinfo)
{
    bool bSummerTime = false;
    if (!pTimeinfo)
        return false;

    time_t t;
    int lastSundayInMarch;
    int lastSundayInOctober;
    struct tm timeinfoStartDst;
    struct tm timeinfoEndDst;

    timeinfoStartDst.tm_year = pTimeinfo->tm_year;
    timeinfoStartDst.tm_mon = 2;
    timeinfoStartDst.tm_mday = GetLastWeekDayInMonth(1, 3, pTimeinfo->tm_year + 1900);
    timeinfoStartDst.tm_hour = 3;
    timeinfoStartDst.tm_min = 0;
    timeinfoStartDst.tm_sec = 0;

    timeinfoEndDst.tm_year = pTimeinfo->tm_year;
    timeinfoEndDst.tm_mon = 9;
    timeinfoEndDst.tm_mday = GetLastWeekDayInMonth(1, 10, pTimeinfo->tm_year + 1900);
    timeinfoEndDst.tm_hour = 2;
    timeinfoEndDst.tm_min = 0;
    timeinfoEndDst.tm_sec = 0;

    //Serial.print("Start summertime: ");
    //printTimeinfoStruct(&timeinfoStartDst);
    //Serial.println();

    //Serial.print("End summertime: ");
    //printTimeinfoStruct(&timeinfoEndDst);
    //Serial.println();
 
    time_t timeinfoVal = timeInfo2time_t(pTimeinfo);
    time_t timeinfoStartDstVal = timeInfo2time_t(&timeinfoStartDst);
    time_t timeinfoEndDstVal = timeInfo2time_t(&timeinfoEndDst);

    if (timeinfoVal > timeinfoStartDstVal && timeinfoVal < timeinfoEndDstVal) {
        bSummerTime = true;
        Serial.print("It's summertime!  ");
    } else {
        bSummerTime = false;
        Serial.print("It's wintertime!  ");
    }
    return bSummerTime;
}




#include "Dusk2Dawn.h"
// hier muss aber in der Originaldatei die Funktion min2Str() und zeroPadTime auf "nicht static" gesetzt werden, sonst gibt es Compilerfehler
// deshalb evtl in lib ordner kopieren und dort anpassen

/// Latitude value for calculating the sunset and sunrise
#define LATITUDE_KIRCHBERG          48.9445
/// Longitude value for calculating the sunset and sunrise
#define LONGITUDE_KIRCHBERG         9.3372
/// Timezone for calculating the sunset and sunrise
#define TIMEZONE_STUTTGART          1.0



// Kirchberg an der Murr / STUTTGART
Dusk2Dawn destination(LATITUDE_KIRCHBERG, LONGITUDE_KIRCHBERG, TIMEZONE_STUTTGART);



int GetSunriseForDateInMin(struct tm* pTimeinfo, bool isDST)
{
    int sunrise  = destination.sunrise(pTimeinfo->tm_year + 1900, pTimeinfo->tm_mon + 1, pTimeinfo->tm_mday, isDST);
    return sunrise;
}



int GetSunsetForDateInMin(struct tm* pTimeinfo, bool isDST)
{
    int sunset  = destination.sunset(pTimeinfo->tm_year + 1900, pTimeinfo->tm_mon + 1, pTimeinfo->tm_mday, isDST);
    return sunset;
}



void printSunSetSunDownForToday()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    
    bool isDST = isSummerTime(&timeinfo);           // check if we have summertime
    int sunrise = GetSunriseForDateInMin(&timeinfo, isDST);
    int sunset  = GetSunsetForDateInMin(&timeinfo, isDST);
    Serial.printf("Sunrise today: %02d:%02d", sunrise / 60, sunrise % 60);
    Serial.printf("   Sunset today: %02d:%02d\n", sunset / 60, sunset % 60);
}



void printTimeinfoStruct(struct tm* pTimeinfo) {
    if (!pTimeinfo)
        return;

    //Serial.println(pTimeinfo, "%A, %B %d %Y %H:%M:%S");           // TODO: Dies führt zu einem Absturz!!!!!!!!!!!!
    Serial.printf("%02d.%02d.%4d %02d:%02d:%02d", pTimeinfo->tm_mday, pTimeinfo->tm_mon + 1, pTimeinfo->tm_year + 1900,
        pTimeinfo->tm_hour, pTimeinfo->tm_min, pTimeinfo->tm_sec);
}



#if !defined(ESP32)
// ist eine Funktion in ESP32 definiert mit default value 5000
// muss für ESP8266 definiert werden
//bool getLocalTime(struct tm * info, uint32_t ms = 5000)
bool getLocalTime(struct tm * info, uint32_t ms)
{
    uint32_t start = millis();
    time_t now;
    while ((millis() - start) <= ms) {
        time(&now);
        localtime_r(&now, info);        // Hier könnte auch KSRTC::time_t2TimeInfo() verwendet werden
        if (info->tm_year > (2016 - 1900)) {
            return true;
        }
   		vTaskDelay(pdMS_TO_TICKS(10));       // warte, bis neue Zeit geschrieben ist. Ansonsten hat getLocalTime eine falsche Zeit
    }
    return false;
}
#endif





time_t getCurrentTime() {
	tm tmInfo = { 0 };
	getLocalTime(&tmInfo);
	//LOGGER.printf("CurrentTime: tm %s\n", KSRTC::getDateTimeISO8601Formated(tmInfo).c_str());
	time_t time = timeInfo2time_t(&tmInfo);

	// test converting from time_t -> tm
	//struct tm ts2;
	//KSRTC::time_t2TimeInfo(&time, &ts2);
	//LOGGER.printf("CurrentTime: time_t %s\n", KSRTC::getDateTimeISO8601Formated(ts2).c_str());

	return time;
}


// convert tm -> time_t
time_t timeInfo2time_t(tm *pTimeInfo) {
    return mktime(pTimeInfo);
}


tm* time_t2TimeInfo(time_t* pTime, tm *pTimeInfo) {
    return localtime_r(pTime, pTimeInfo);
    //return localtime(pTime);
}

