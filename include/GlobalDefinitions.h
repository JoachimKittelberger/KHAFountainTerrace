
#pragma once

// evtl sinnvolle DSL APIs


typedef struct {
    bool isOn = false;              // LED on or off
    int brightness = 100;           // 0 .. 255
    uint32_t color;                 // RGB
    LightEffect lightEffect = LightEffect::LEBlack;
    int lightEffectSpeed = 100;        // Geschwindigkeit des Effects in ms (20 .. max)
} NeoPixelLED;



typedef struct {
    bool isSolarOn = true;         // Solar on or off
    bool isFontainOn = false;       // Fontain on or off
    int height = 0;      // Height of fontain 0 .. 255

} Fontain;


typedef struct {
    NeoPixelLED ledBrunnen;
    NeoPixelLED ledRing;
    NeoPixelLED ledStufe;
    Fontain fontain;
} BrunnenData;



/*
// JSON-Document für WebService und Kommunikation mit html-page und mqtt
{
    "ledbrunnen": {
        "mode":"on",
        "brightness":100,
        "colorRGB": "#0433ff",
        "lighteffect":"LEDBlack",       // hier eventuell mit Zahlen arbeiten oder Liste mit FLASHHELPER Array und Texten durchgehen und Index ermitteln
        "lespeed":100
    },
    "ledring": {
    ...
    },
    "ledstufe": {
    ...
    },
    
    "fontain": {
        "solar":"on",
        "mode":"on",
        "height":150
    }
}
*/





void initModeRelaisModul();
void switchToSolarMode(bool bSolarMode = true);
void switchToPowerMode();
bool isInSolarMode();

void initFontain();
void setFontainOn(bool bOn = true);
void setFontainOff();
bool isFontainOn();
void setFontainHeight(int height = 255);        // maximum hight = 255. Minimum Hight = 150? Eventuell hier in % arbeiten
int getFontainHeight();



/*

// homekit mit RGB-LED implementieren

// LEDs:
void setLEDOn(bool bOn = true);
void setLEDOff();

void setBrightness(int brightness);          // vermutlich nur von 0-255 evtl. hier in Prozent angeben
void setEffectMode(enum EffectModes mode);   // hier die verschiedenen Effekte angeben als Enum
*/

// LED-Ansteuerung in einen separaten Thread auslagern.
// Das ganze über MQTT ansteuerbar machen
// Das ganze über webserver ansteurbar machen. (Homepage)

// das ganze über Hue oder Homekit steuerbar machen







