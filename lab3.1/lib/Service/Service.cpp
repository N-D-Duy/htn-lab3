#include "Service.h"
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <time.h>
#include "config.h"

// WiFi credentials
const char *ssid = "Wokwi-GUEST";
const char *password = "";
#define WIFI_CHANNEL 6

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String streamPath = "/lab3-1";

Service::Service()
{
    Serial.begin(115200);
}

void Service::getFirebaseData()
{
    FirebaseData fbdo;
    FirebaseJson jsonData;
    if (Firebase.RTDB.getJSON(&fbdo, streamPath.c_str()))
    {
        if(fbdo.dataType() != "null"){
            jsonData.setJsonData(fbdo.jsonString());
            FirebaseJsonData result;
            if(jsonData.get(result, "humidity") && result.type == "float"){
                humidity = result.floatValue;
            } 

            if(jsonData.get(result, "temperature") && result.type == "float"){
                temperature = result.floatValue;
            }

            if(jsonData.get(result, "isPumpOn") && result.type == "boolean"){
                isPumpOn = result.boolValue;
            }
        }
    }
    else
    {
        Serial.println(fbdo.errorReason());
    }
}

void Service::initService()
{
    Serial.println("Initializing service...");
    if(API_KEY == "" || DATABASE_URL == "" || USER_EMAIL == "" || USER_PASSWORD == "") {
        Serial.println("Credentials is not set!");
        return;
    }
    int wifiTimeout = 30;
    WiFi.begin(ssid, password, WIFI_CHANNEL);

    while (WiFi.status() != WL_CONNECTED && wifiTimeout > 0) {
        delay(500);
        Serial.print(".");
        wifiTimeout--;
    }
    Serial.println("\nConnected to WiFi");

    // Firebase configuration
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    Firebase.reconnectWiFi(true);
    fbdo.setResponseSize(4096);
    config.token_status_callback = tokenStatusCallback;
    config.max_token_generation_retry = 5;

    Firebase.begin(&config, &auth);

    configTime(0, 0, "pool.ntp.org");
    delay(2000);
}

void Service::turnPumpOn(){
    FirebaseJson jsonData;
    jsonData.add("isPumpOn", true);
    if(Firebase.RTDB.updateNode(&fbdo, streamPath.c_str(), &jsonData)){
        Serial.println("Pump is turned on via firebase");
    } else {
        Serial.println(fbdo.errorReason());
    }
}

void Service::turnPumpOff(){
    FirebaseJson jsonData;
    jsonData.add("isPumpOn", false);
    if(Firebase.RTDB.updateNode(&fbdo, streamPath.c_str(), &jsonData)){
        Serial.println("Pump is turned off via firebase");
    } else {
        Serial.println(fbdo.errorReason());
    }
}

void Service::setHumidity(float humidity){
    FirebaseJson jsonData;
    jsonData.add("humidity", humidity);
    if(Firebase.RTDB.updateNode(&fbdo, streamPath.c_str(), &jsonData)){
        // Serial.println("Humidity is updated via firebase");
    } else {
        Serial.println(fbdo.errorReason());
    }
}

void Service::setTemperature(float temperature){
    FirebaseJson jsonData;
    jsonData.add("temperature", temperature);
    if(Firebase.RTDB.updateNode(&fbdo, streamPath.c_str(), &jsonData)){
        // Serial.println("Temperature is updated via firebase");
    } else {
        Serial.println(fbdo.errorReason());
    }
}