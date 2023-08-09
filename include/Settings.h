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
 * MIT License
 *
 * Copyright (c) 2023 Joachim Kittelberger - KibeSoft, www.kibesoft.de
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "GlobalDefinitions.h"


#define PREFKEY_FOUNTAIN_TERRACE "fountainterrace"


class Settings {
    public:
        Settings(BrunnenData* pBrunnen);
    
    public:
        bool saveCurrentSettingsToMemory();
        bool loadCurrentSettingsFromMemory();
        void deleteSettingsFromMemory();
        bool saveCurrentSettingsToMemoryIfChanged();

        void changed(bool changed = true) {
            _hasChanged = changed;
        }
        bool hasChanged() { return _hasChanged; }

    private:
        BrunnenData* _pBrunnen = NULL;
        bool _hasChanged = false;
};


#endif          // #define _SETTINGS_H