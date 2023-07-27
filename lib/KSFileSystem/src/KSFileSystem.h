#ifndef _KSFILESYSTEM_H
#define _KSFILESYSTEM_H


// TODO for new releases of LittleFS-Library
// comment //#define CONFIG_LITTLEFS_FOR_IDF_3_2 in esp_littlefs.c
// 30.07.2022: Wird vermutlich nicht mehr benötigt

// For use with SPIFFS
#define KSUSE_LittleFS

#ifdef KSUSE_LittleFS
  	#define SPIFFS LITTLEFS        // TODO: we got an error if we use this. Don't know if we uncomment this again

	// benötigen wir nur, weill der Firebeetle noch keine Unterstützung für LittleFS hat
	#ifdef KSUSE_OLDFS_FOR_FIREBEETLE
		#include <LITTLEFS.h> 
	  	#define LittleFS LITTLEFS
        #warning Use LITTLEFS only in projects with Firebeetle and weatherstation in /Users/CloudStation/Privat/Projekte/ESP32/libraries/KSLibraries/KSAutoUpdate/KSAutoUpdate.h
	#else
		#include <LittleFS.h>
        //#warning Using LittleFS for current project
	#endif
#else
  	//#define LITTLEFS SPIFFS
  	#define LittleFS SPIFFS
   	#include <SPIFFS.h>
    #warning Using SPIFFS for current project
#endif


//#define KSFileSystem LITTLEFS
#define KSFileSystem LittleFS			// TODO: Evtl. hier eine Klasse draus machen

/* You only need to format LITTLEFS the first time you run a
   test or else use the LITTLEFS plugin to create a partition
   https://github.com/lorol/arduino-esp32littlefs-plugin */
   
#define FORMAT_KSFILESYSTEM_IF_FAILED true



void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);

void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);

void testFS();


#endif  // #define _KSFILESYSTEM_H
