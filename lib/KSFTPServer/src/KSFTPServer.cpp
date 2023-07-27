#include "KSFTPServer.h"
#include "KSEventGroupNetwork.h"

#include "KSFileSystem.h"



//extern const char ftpUserName[];
//extern const char ftpUserPassword[];



KSFTPServer::KSFTPServer() : ftpSrv(KSFileSystem) {
}


KSFTPServer::~KSFTPServer() {
}




TaskHandle_t KSFTPServer::createConnection(EventGroupHandle_t *phEventGroupNetwork) {

	_phEventGroupNetwork = phEventGroupNetwork;

	int coreID = xPortGetCoreID();
	//Serial.print(F("CoreID: "));
	//Serial.println(coreID);
	
	UBaseType_t setupPriority = uxTaskPriorityGet(NULL);
	//Serial.print(F("setup: priority = "));
	//Serial.println(setupPriority);

	xTaskCreatePinnedToCore(
        [](void* context){ static_cast<KSFTPServer*>(context)->tKSFTPServer(); },
		"tKSFTPServer",
		8192,
		this,
		setupPriority,
		&_htKSFTPServer,
		coreID
		);

	return _htKSFTPServer;
}




void KSFTPServer::tKSFTPServer()
{
    // Warte auf Wifi-Verbindung und Initialisierung:
    // Wenn Connection-Bit noch nicht gesetzt, dann das erste Mal warten.
    if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
        if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
            //Serial.println(F("[ftp] Wating for Event EG_NETWORK_CONNECTED"));
            EventBits_t eventGroupValue;
            eventGroupValue = xEventGroupWaitBits(*_phEventGroupNetwork, (EG_NETWORK_INITIALIZED | EG_NETWORK_CONNECTED), pdFALSE, pdTRUE, portMAX_DELAY);
            //Serial.println(F("[ftp] Event EG_NETWORK_CONNECTED set"));
        }
    }
            
    // erst hier machen, da sonst Absturz. Vermutlich wegen Wifi noch nicht initialisiert.
    //ftpSrv.begin(ftpUserName, ftpUserPassword);
    ftpSrv.begin(_pUsername, _pPassword);
	
	// main loop ftp
    for (;;) {

        // Warte auf Wifi-Verbindung und Initialisierung:
        // Wenn Connection-Bit noch nicht gesetzt, dann das erste Mal warten.
        if (_phEventGroupNetwork && (*_phEventGroupNetwork != NULL)) {
            if ((xEventGroupGetBits(*_phEventGroupNetwork) & EG_NETWORK_CONNECTED) == 0) {
                //Serial.println(F("[ftp] Wating for Event EG_NETWORK_CONNECTED"));
                EventBits_t eventGroupValue;
                eventGroupValue = xEventGroupWaitBits(*_phEventGroupNetwork, (EG_NETWORK_INITIALIZED | EG_NETWORK_CONNECTED), pdFALSE, pdTRUE, portMAX_DELAY);
                //Serial.println(F("[ftp] Event EG_NETWORK_CONNECTED set"));
            }
        }

        ftpSrv.handleFTP();
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}



