/**
 * @file KSFileSystem.h
 * 
 * @brief Header file for KSFileSystemClass class
 * 
 * @details 
 * 
 * @see
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

#ifndef _KSFILESYSTEM_H
#define _KSFILESYSTEM_H


// TODO for new releases of LittleFS-Library
// comment //#define CONFIG_LITTLEFS_FOR_IDF_3_2 in esp_littlefs.c
// 30.07.2022: Wird vermutlich nicht mehr benötigt

// the next lines are for compatibility of old projects
#define KSFileSystem LittleFS			// Evtl. hier eine Klasse draus machen

// For use with LittleFS and SPIFFS
#define KSUSE_LittleFS
#ifdef KSUSE_LittleFS
	// benötigen wir nur, weill der Firebeetle noch keine Unterstützung für LittleFS hat
	#ifdef KSUSE_OLDFS_FOR_FIREBEETLE
		#include <LITTLEFS.h> 
	  	#define LittleFS LITTLEFS
  		#define SPIFFS LITTLEFS        // TODO: we got an error if we use this. Don't know if we uncomment this again
        #warning Use LITTLEFS only in projects with Firebeetle and weatherstation in /Users/CloudStation/Privat/Projekte/ESP32/libraries/KSLibraries/KSAutoUpdate/KSAutoUpdate.h
	#else
		#include <LittleFS.h>
  		//#define SPIFFS LittleFS        // TODO: we got an error if we use this. Don't know if we uncomment this again
        //#warning Using LittleFS for current project
	#endif
#else
  	//#define LITTLEFS SPIFFS
  	#define LittleFS SPIFFS
   	#include <SPIFFS.h>
    #warning Using SPIFFS for current project
#endif




//#define DISABLE_ALL_LIBRARY_WARNINGS

// code is from esp32FOTA.hpp and modified to get LittleFS as Default
#include <FS.h>

// inherit includes from sketch, detect SPIFFS first for legacy support
#if __has_include(<LittleFS.h>) || defined _LiffleFS_H_
  #if !defined(DISABLE_ALL_LIBRARY_WARNINGS)
  #pragma message "Using LittleFS for KSFileSystem"
  #endif
  #include <LittleFS.h>
  #define KSFS &LittleFS
#elif __has_include(<SPIFFS.h>) || defined _SPIFFS_H_
  #if !defined(DISABLE_ALL_LIBRARY_WARNINGS)
  #pragma message "Using SPIFFS for KSFileSystem"
  #endif
  #include <SPIFFS.h>
  #define KSFS &SPIFFS
#elif __has_include(<SD.h>) || defined _SD_H_
  #if !defined(DISABLE_ALL_LIBRARY_WARNINGS)
  #pragma message "Using SD for KSFileSystem"
  #endif
  #include <SD.h>
  #define KSFS &SD
#elif __has_include(<SD_MMC.h>) || defined _SD_MMC_H_
  #if !defined(DISABLE_ALL_LIBRARY_WARNINGS)
  #pragma message "Using SD_MMC for KSFileSystem"
  #endif
  #include <SD_MMC.h>
  #define KSFS &SD_MMC
#elif defined _LIFFLEFS_H_ // older externally linked, hard to identify and unsupported versions of SPIFFS
  #if !defined(DISABLE_ALL_LIBRARY_WARNINGS)
  #pragma message "this version of LittleFS is unsupported, use #include <LittleFS.h> instead, if using platformio add LittleFS(esp32)@^2.0.0 to lib_deps"
  #endif
#elif __has_include(<PSRamFS.h>) || defined _PSRAMFS_H_
  #if !defined(DISABLE_ALL_LIBRARY_WARNINGS)
  #pragma message "Using PSRamFS for KSFileSystemn"
  #endif
  #include <PSRamFS.h>
  #define KSFS &PSRamFS
#else
  //#if !defined(DISABLE_ALL_LIBRARY_WARNINGS)
  // #pragma message "No filesystem provided, certificate validation will be unavailable (hint: include SD, SPIFFS or LittleFS before including this library)"
  //#endif
  #define KSFS nullptr
#endif





class KSFileSystemClass {
	public:
		KSFileSystemClass(fs::FS& filesystem = *KSFS) { _fs = &filesystem; }
		~KSFileSystemClass() {}

		fs::FS& getKSFileSystemRef() { return *_fs; }

		bool init(bool formatIfMountFailed = true);
		bool format();

		void listDir(const char * dirname, uint8_t levels);
		void createDir(const char * path);
		void removeDir(const char * path);
		void readFile(const char * path);

		void writeFile(const char * path, const char * message);
		void appendFile(const char * path, const char * message);
		void renameFile(const char * path1, const char * path2);
		void deleteFile(const char * path);

	private:
  		fs::FS* _fs = KSFS; // default filesystem

};




// some testfunctions
void testFileIO(fs::FS &fs, const char * path);


#endif  // #define _KSFILESYSTEM_H
