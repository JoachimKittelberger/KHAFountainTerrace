#pragma once


/* Using:

// declaraion of object
KSResetController resetController(3*24*60*60);

// callback function
void onReset() {
  // is called before Reset of ESP32
  Serial.println("Reseting ESP32 ...");
  Serial.flush();
  vTaskDelay(pdMS_TO_TICKS(100));
}

// in setup()
  //resetController.setResetTime(3*24*60*60);
  resetController.setOnResetListener(onReset);
  resetController.create();

*/


// Reset the Controller after a defined period of time
// max unsigned long = 4,294,967,295 = 71582 min = 1193h = 49d

#define MAX_ULONG 4294967295
#define INIT_RESETTIME_IN_SEC (3 * 24 * 60 * 60)        // 3days


class KSResetController
{
    public:
        KSResetController(unsigned long resetTimeInSec = INIT_RESETTIME_IN_SEC);
        ~KSResetController();
  
        TaskHandle_t create();
        
        void setResetTime(unsigned long resetTimeInSec) {
            _resetTimeInMillis = resetTimeInSec * 1000;
        }
        static void reset();

        void setOnResetListener(void (*callback)()) { onResetListener = callback; }

    
    protected:
       void (*onResetListener)();

 
    private:
        void tKSResetController();
        TaskHandle_t _htKSResetController;

        unsigned long _resetTimeInMillis;
};
