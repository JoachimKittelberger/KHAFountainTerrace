/**
 * @file Settings.h
 *
 * @brief Header file for using Settings class
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
#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "GlobalDefinitions.h"
#include "KSPreferences.h"			// wird nur benötigt, wenn in eigener Datei ausgelagert


#define PREFKEY_FOUNTAIN_TERRACE "fountainterrace"


class Settings {
    public:
        Settings(BrunnenData* pBrunnen);
    
    public:
        bool saveCurrentSettingsToMemory();
        bool loadCurrentSettingsFromMemory();
        void deleteSettingsFromMemory();

        void changed(bool changed = true) {
            _hasChanged = changed;
        }
        bool hasChanged() { return _hasChanged; }

    private:
        BrunnenData* _pBrunnen = NULL;
        bool _hasChanged = false;
};


#endif          // #define _SETTINGS_H