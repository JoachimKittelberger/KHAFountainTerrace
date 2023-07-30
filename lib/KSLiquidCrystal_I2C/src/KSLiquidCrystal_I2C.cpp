/**
 * @file KSLiquidCrystal_I2C.cpp
 * 
 * @brief implementation for KSLiquidCrystal_I2C
 * 
 * @details 
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

#include "KSLiquidCrystal_I2C.h"


KSLiquidCrystal_I2C::KSLiquidCrystal_I2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows, KSCriticalSection* pcsI2C)
        : LiquidCrystal_I2C(lcd_Addr, lcd_cols, lcd_rows) {

    _pcsI2C = pcsI2C;
}






