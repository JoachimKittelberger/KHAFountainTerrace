/**
 * @file KSShelly.h
 * 
 * @brief controll shelly
 * 
 * @details implementation of KSShelly class
 * 
 * @see
 * 
 * @author Joachim Kittelberger <jkittelberger@kibesoft.de>
 * @date 30.04.2022
 * @version 1.00
 *
 * @todo
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


#include "KSShelly.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>





/*
// JSON um über mqtt den shelly zu schalten bzw. Status auszuwerten
	home/Terrasse/events			// wird beim ein/ausschalten versendet, egal woher geschaltet wird
	rpc = {
		"src":"shellyplus1-441793a5d2fc",
		"dst":"home/Terrasse/events",
		"method":"NotifyStatus",
		"params":{
			"ts":1652704957.52,
			"switch:0":{
				"id":0,
				"output":true,
				"source":"HTTP"
			}
		}
	}
	
	home/Terrasse/status			// wird bei einer status-Änderung versendet. source kann dabei auch MQTT sein
	switch:0 = {
		"id":0,
		"source":"HTTP",
		"output":true,
		"temperature":{
			"tC":54.0,
			"tF":129.3
		}
	}

	Send command:
	home/Terrasse/rpc
	Einschalten mit: home/Terrasse
	rpc = {
		"id":123,				// eigene ID der Hardware
		"src": "user_1",		// Benutzer-ID. 
		"method": "Switch.Set",
		"params": {
			"id": 0,
			"on":true
		}
	}

*/






// setzte den Shelly auf den neuen state und liefert den alten state zurück
// http.begin("http://192.168.1.10/rpc/Switch.Set?id=0&on=true"); //HTTP

bool KSShelly::setState(bool bOn, int id) {

    bLastError = false;
    String serverPath = "http://" + _shellyName;
    //serverPath.concat(shellyName);
    serverPath.concat("/rpc/Switch.Set?id=");
    serverPath.concat(id);
    serverPath.concat("&on=");
    serverPath.concat(bOn ? "true" : "false");
    //Serial.println(serverPath);

    String payload = httpGETRequest(serverPath.c_str());

    // convert payload
	StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print("Error: KSShelly::setState(...) deserializeJson() failed: ");
        Serial.println(error.c_str());
        bLastError = true;
        return false;
    }

    bool lastState = doc["was_on"].as<bool>() == true;
    return lastState;
}




String KSShelly::httpGETRequest(const char* requestURL) {
    HTTPClient http;
    
    // Your IP address with path or Domain name with URL path 
    //Serial.print("[HTTP] begin...\n");
    http.begin(requestURL);
    
    // Send HTTP POST request
    int httpResponseCode = http.GET();
    
    String payload = "{}"; 
    
    if (httpResponseCode > 0) {
        //Serial.printf("[HTTP] GET... code: %d\n", httpResponseCode);

        if (httpResponseCode == HTTP_CODE_OK) {
            payload = http.getString();
            //Serial.println(payload);
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error %d: %s\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());
        bLastError = true;
    }

    // Free resources
    http.end();
    
    return payload;
}



// geht noch über http POST (wie curl) und mqtt

/* HTTP POST
String httpPOSTRequest(const char* serverName) {

      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=aPnTH7Ab3k9G5&sensor=BME280&value1=36.00&value2=60.54&value3=954.14";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"api_key\":\"aPnTH7Ab3k9G5\",\"sensor\":\"BME280\",\"value1\":\"36.00\",\"value2\":\"60.54\",\"value3\":\"954.14\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();


      return 
}

*/