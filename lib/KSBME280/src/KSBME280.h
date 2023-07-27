#ifndef _KSBME280_H
#define _KSBME280_H

#include <Adafruit_BME280.h>
#include "KSInterfaces.h"
#include "KSCriticalSection.h"


#define KS_BME_SDA 21
#define KS_BME_SCL 22


#define SEALEVELPRESSURE_HPA (1013.25)


// save i2c access with critical section
class KSAdafruit_BME280 : public Adafruit_BME280 {
    public:

friend class KSBME280;

        KSAdafruit_BME280(KSCriticalSection* pcsI2C = NULL) : Adafruit_BME280() {
            _pcsI2C = pcsI2C;
        }
        virtual ~KSAdafruit_BME280() {}

        bool begin(uint8_t addr = BME280_ADDRESS, TwoWire *theWire = &Wire) {
            return Adafruit_BME280::begin(addr, theWire);
        } 


        void sleep() {
            if (_pcsI2C) _pcsI2C->EnterCriticalSection();
            Adafruit_BME280::write8(BME280_REGISTER_CONTROL, 0x3C);

/*            if (_pWire) {
                const uint8_t CTRL_MEAS_REG = 0xF4;
                // BME280 Register 0xF4 (control measurement register) sets the device mode, specifically bits 1,0
                // The bit positions are called 'mode[1:0]'. See datasheet Table 25 and Paragraph 3.3 for more detail.
                // Mode[1:0]  Mode
                //    00      'Sleep'  mode
                //  01 / 10   'Forced' mode, use either '01' or '10'
                //    11      'Normal' mode
                Serial.println("BME280 to Sleep mode...");
                _pWire->beginTransmission(_I2Caddr);
                _pWire->requestFrom(_I2Caddr, 1);
                uint8_t value = _pWire->read();
                value = (value & 0xFC) + 0x00;         // Clear bits 1 and 0
                _pWire->write((uint8_t)CTRL_MEAS_REG);    // Select Control Measurement Register
                _pWire->write((uint8_t)value);            // Send 'XXXXXX00' for Sleep mode
                _pWire->endTransmission();
            }
*/            if (_pcsI2C) _pcsI2C->LeaveCriticalSection();
        }

        void wakeup() {
            if (_pcsI2C) _pcsI2C->EnterCriticalSection();
            Adafruit_BME280::write8(BME280_REGISTER_CONTROL, 0x3F);
            if (_pcsI2C) _pcsI2C->LeaveCriticalSection();
        }



//       bool sleep() {
/*
            // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            _bme680.power_mode = BME680_SLEEP_MODE;

            if (bme680_set_sensor_mode(&_bme680) != BME680_OK) {
                Serial.println("[KSBsec]: Error bme680_set_sensor_mode()");
                return false;
            }
*/
//            return true;
//        }
/*
    private:    
        static int8_t i2cRead(uint8_t devId, uint8_t regAddr, uint8_t *regData, uint16_t length) {
            //Serial.println("i2cRead");
            uint8_t retVal = 0;
            if (_pcsI2C) _pcsI2C->EnterCriticalSection();
            retVal = Bsec::i2cRead(devId, regAddr, regData, length);
            if (_pcsI2C) _pcsI2C->LeaveCriticalSection();
            return retVal;
        }

    
        static int8_t i2cWrite(uint8_t devId, uint8_t regAddr, uint8_t *regData, uint16_t length) {
            //Serial.println("i2cWrite");
            uint8_t retVal = 0;
            if (_pcsI2C) _pcsI2C->EnterCriticalSection();
            retVal = Bsec::i2cWrite(devId, regAddr, regData, length);
            if (_pcsI2C) _pcsI2C->LeaveCriticalSection();
            return retVal;
        }
        */
    private:
        KSCriticalSection* _pcsI2C = NULL;;

};







class KSBME280 : public IKSTemperature, public IKSHumidity
{
    public:
        KSBME280(uint8_t addr = BME280_ADDRESS, TwoWire *theWire = &Wire, KSCriticalSection* pcsI2C = NULL);
        ~KSBME280() {};
  
        void init();

        // Implementation of Interface IKSTemperature
        virtual float getTemperature() {
            return readTemperature();
        };
        // Implementation of Interface IKSHumidity
        virtual float getHumidity() {
            return readHumidity();
        }



        float readTemperature();
        float readPressure();
        float readHumidity();
        float readAltitude(float seaLevel = SEALEVELPRESSURE_HPA);
        bool read(float* pTemperature = NULL, float* pHumidity = NULL, float* pPressure = NULL, float* pAltitude = NULL);

        void sleep() {
            _bme.sleep();
        }

 
    private:
        //Adafruit_BME280 _bme;
        KSAdafruit_BME280 _bme;
        uint8_t _I2Caddr = BME280_ADDRESS;
        TwoWire* _pWire = &Wire;

        KSCriticalSection* _pcsI2C = NULL;
        bool _isInitialized = false;
};


#endif  // #define _KSBME280_H