#pragma once

#include <Preferences.h>





class KSPreferences : public Preferences {
    public:
        KSPreferences() : Preferences() {}
        virtual ~KSPreferences() { }

        void eraseNVSMemory();           // erase the complete NVS-Memory
        void putKSValues();              // initialize all KSValues


    protected:
 
    private:
};




/*
	preferences für credentials wifi, mqtt, ....... generell in einem Setup-Programm anlegen. Evtl. eine KSPreferences-Lib mit Anlegen, löschen, ...


	preferences.begin("weatherstation", false);
	//preferences.clear();	// clear all data under namespace
	//preferences.remove(key);	// remove key from an open namespace

	unsigned int counter = preferences.getUInt("counter", 0);
	counter++;
	preferences.putUInt("counter", counter);
	preferences.end();				// close the opened namespace
*/

