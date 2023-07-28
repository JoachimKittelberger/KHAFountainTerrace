/**
 * @file KSUtilities.h
 * 
 * @brief Header file for using KSUtilities functions
 * 
 * @details 
 * 
 * @see
 *   - https://github.com/dmkishi/Dusk2Dawn.git @ 1.0.1
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

#ifndef _KSUTILITIES_H
#define _KSUTILITIES_H


#include <Arduino.h>


// is now in ESP SDK
//#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__) // For Windows use \\ instead of /

// check, if an string starts with an given prefixstring
//#define STARTS_WITH(string_to_check, prefix) (strncmp(string_to_check, prefix, ((sizeof(prefix) / sizeof(prefix[0])) - 1)) ? 0:((sizeof(prefix) / sizeof(prefix[0])) - 1))
bool strStartsWith(const char *str, const char *prefix);

#define US2SECOND 1000000ULL            // Conversion factor for micro seconds to seconds
#define MS2SECOND 1000UL                // Conversion factor for milli seconds to seconds


// define Macros for loops dependent on cycletime
//	static unsigned long startloopNameMillis = millis();
//	unsigned long currentloopNameMillis = millis();
//	if ((currentloopNameMillis - startloopNameMillis) >= cycletime) {
//		startloopNameMillis = currentloopNameMillis;
//      ...
//	}
#define BEGIN_CYCLIC(loopName, cycleTime) \
  static unsigned long start ## loopName ## Millis = millis(); \
  unsigned long current ## loopName ## Millis = millis(); \
  if ((current ## loopName ## Millis - start ## loopName ## Millis) >= (cycleTime)) { \
    start ## loopName ## Millis = current ## loopName ## Millis;

#define END_CYCLIC() }



void formatDATE2DE(char const *date, char *buff, size_t len);
void printSketchInfo(char const* currentProject);


// CRC-Brechnung für Insta -> Evtl. in Instal-Klasse übernehmen
uint8_t calculateCRCXOR(uint8_t* pBuffer, int count);

char getHexHigh(uint8_t value);
char getHexLow(uint8_t value);


// DateTime Fuctions
int GetNumberOfDaysPerMonth(int month, int year);
int GetDayOfWeekForDate(int day, int month, int year);
int GetLastWeekDayInMonth(int weekDay, int month, int year);

bool isSummerTime(struct tm* pTimeinfo);

int GetSunriseForDateInMin(struct tm* pTimeinfo, bool isDST);
int GetSunsetForDateInMin(struct tm* pTimeinfo, bool isDST);
void printSunSetSunDownForToday();
void printTimeinfoStruct(struct tm* pTimeinfo);

#if !defined(ESP32)
bool getLocalTime(struct tm * info, uint32_t ms = 5000)
#endif

time_t getCurrentTime();
time_t timeInfo2time_t(tm *pTimeInfo);
tm* time_t2TimeInfo(time_t* pTime, tm *pTimeInfo);




#endif // #define _KSUTILITIES_H