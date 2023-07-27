#include "KSResetController.h"


KSResetController::KSResetController(unsigned long resetTimeInSec) {
	onResetListener = NULL;
	_resetTimeInMillis = resetTimeInSec * 1000;
	//create();
}


KSResetController::~KSResetController() {

}
  
  
TaskHandle_t KSResetController::create() {
	int coreID = xPortGetCoreID();
	UBaseType_t setupPriority = uxTaskPriorityGet(NULL);

	xTaskCreatePinnedToCore(
    	[](void* context){ static_cast<KSResetController*>(context)->tKSResetController(); },
		"tKSResetController",
		8192,
		this,
		setupPriority,
		&_htKSResetController,
		coreID
		);

	return _htKSResetController;
}
        
 


void KSResetController::tKSResetController() {
	Serial.println("created Task tKSResetController");
	for (;;) {
		unsigned long currentMillis = millis();
		if (currentMillis >= _resetTimeInMillis) {

			Serial.print("ResetController reset after ");
			Serial.print(currentMillis);
			Serial.println("ms. Restarting ESP ...");
			Serial.flush();
			
			if (onResetListener) {
				onResetListener();
			}
			reset();
		} else {
/*			Serial.print("Check Resetcontroller current ms: ");
			Serial.print(currentMillis);
			Serial.print(" Reset at: ");
			Serial.println(_resetTimeInMillis);
*/		}
		vTaskDelay(pdMS_TO_TICKS(1000));

	}	
}



void KSResetController::reset() {
	yield();
	vTaskDelay(pdMS_TO_TICKS(100));
	yield();
	ESP.restart();
}
