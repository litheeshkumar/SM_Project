#include <ESP8266WiFi.h>

// WiFi parameters
#define WLAN_SSID       "Redmi 5A"
#define WLAN_PASS       "k.pallavi"

int frequency=1000; //Specified in Hz
int buzzPin=2;//D4 GPIO2 
int timeOn=1000; //specified in milliseconds
int timeOff=1000; //specified in millisecods


void setup() {
  // put your setup code here, to run once:
  
    Serial.begin(115200);
    Serial.println(F("Adafruit IO Example"));
    // Connect to WiFi access point.
    Serial.println(); Serial.println();
    delay(10);
    Serial.print(F("Connecting to "));
    Serial.println(WLAN_SSID);
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(F("."));
    }
    Serial.println();
    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());


}

void loop(){
 
   tone(buzzPin, frequency);
   delay(timeOn);
   noTone(buzzPin);
   delay(timeOff);
}
