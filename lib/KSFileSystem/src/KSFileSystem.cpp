/**
 * @file KSFileSystem.cpp
 * 
 * @brief Implementation for KSFileSystemClass class
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
 *   - Add other FileSystems to init (not just LittleFS)
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
#include "KSFileSystem.h"



bool KSFileSystemClass::init(bool formatIfMountFailed) {
    if (!_fs) return false;
    
    // Initialize file system
 
    // initialize LittleFS
    // TODO: handle other FileSystems
    if (_fs == &LittleFS) {
        Serial.print("Initialize FileSystem LittleFS... ");
        fs::LittleFSFS* pFS = static_cast<fs::LittleFSFS*>(_fs);
        if (pFS) {
            if (!pFS->begin(false)) { // Do not format if mount failed
                if (formatIfMountFailed) {
                    Serial.print("failed... trying to format...");
                    if (!pFS->begin(true)) {
                        Serial.println("success");
                    } else {
                        Serial.println("failed");
                        return false;
                    }
                } else {
                    Serial.println("failed");
                    return false;
                }
            } else {
                Serial.println("done");
            }
        }
        return true;
    }

    return false;
}



bool KSFileSystemClass::format() {
    if (!_fs) return false;

    if (_fs == &LittleFS) {
        Serial.print("Formating FileSystem... ");
        fs::LittleFSFS* pFS = static_cast<fs::LittleFSFS*>(_fs);
        if (pFS) {
            if (pFS->format()) {
                Serial.println("done");
                return true;
            } else {
                Serial.println("failed");
                return false;
            }
        }
    }

    return false;
}



void KSFileSystemClass::listDir(const char * dirname, uint8_t levels) {
    if (!_fs) return;
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = _fs->open(dirname);
    if (!root){
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                String path = dirname;
                path += file.name();
                listDir(path.c_str(), levels -1);
                //listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}



void KSFileSystemClass::createDir(const char * path) {
    if (!_fs) return;
    Serial.printf("Creating Dir: %s\n", path);
    if (_fs->mkdir(path)) {
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}



void KSFileSystemClass::removeDir(const char * path) {
    if (!_fs) return;
    Serial.printf("Removing Dir: %s\n", path);
    if (_fs->rmdir(path)) {
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}



void KSFileSystemClass::readFile(const char * path) {
    if (!_fs) return;
    Serial.printf("Reading file: %s\r\n", path);

    File file = _fs->open(path);
    if (!file || file.isDirectory()) {
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}



void KSFileSystemClass::writeFile(const char * path, const char * message) {
    if (!_fs) return;
    Serial.printf("Writing file: %s\r\n", path);

    File file = _fs->open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}



void KSFileSystemClass::appendFile(const char * path, const char * message) {
    if (!_fs) return;
    Serial.printf("Appending to file: %s\r\n", path);

    File file = _fs->open(path, FILE_APPEND);
    if (!file) {
        Serial.println("- failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}



void KSFileSystemClass::renameFile(const char * path1, const char * path2) {
    if (!_fs) return;
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (_fs->rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}



void KSFileSystemClass::deleteFile(const char * path) {
    if (!_fs) return;
    Serial.printf("Deleting file: %s\r\n", path);
    if (_fs->remove(path)) {
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}













// some Test-functions
void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for (i=0; i<2048; i++) {
        if ((i & 0x001F) == 0x001F) {
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if (file && !file.isDirectory()) {
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len) {
            size_t toRead = len;
            if (toRead > 512) {
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F) {
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
    }
}



