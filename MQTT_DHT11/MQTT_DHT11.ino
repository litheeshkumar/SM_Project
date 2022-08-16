#include <SimpleDHT.h>      // Data ---> D3
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// WiFi parameters
#define WLAN_SSID       "Redmi 5A"
#define WLAN_PASS       "k.pallavi"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "Litheesh14"
#define AIO_KEY         "aio_PqSR55Pa9OVypEN4NBComN0TS1fu" 
WiFiClient client;
#define Buzzer_pin 2

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temp");
Adafruit_MQTT_Publish Ecg = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ecg");
Adafruit_MQTT_Publish BP = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bp");
Adafruit_MQTT_Publish HP = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/hp");

int pinDHT11 = 0;
SimpleDHT11 dht11(pinDHT11);
byte temp = 0; //Stores temperature value

// connect to adafruit io via MQTT
void connect() 
{
    Serial.print(F("Connecting to Adafruit IO... "));
    int8_t ret;
    while ((ret = mqtt.connect()) != 0) 
    {
        switch (ret) 
        {
            case 1: Serial.println(F("Wrong protocol")); break;
            case 2: Serial.println(F("ID rejected")); break;
            case 3: Serial.println(F("Server unavail")); break;
            case 4: Serial.println(F("Bad user/pass")); break;
            case 5: Serial.println(F("Not authed")); break;
            case 6: Serial.println(F("Failed to subscribe")); break;
            default: Serial.println(F("Connection failed")); break;
        }
        if(ret >= 0)
            mqtt.disconnect();
        Serial.println(F("Retrying connection..."));
        delay(10000);
    }
    Serial.println(F("Adafruit IO Connected!"));
}

void setup() 
{
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

    // connect to adafruit io
    connect();
}

void loop()   // ping adafruit io a few times to make sure we remain connected 
{
    if(! mqtt.ping(3))    // reconnect to adafruit io 
    {
        if(! mqtt.connected())
        connect();
    }
    dht11.read(&temp, NULL, NULL);
    //temp = random(34, 37);
    Serial.print("Temprature = ");
    Serial.print((int)temp);
    Serial.println("C");

    float ecg = random(2, 10);
    ecg = ecg/10;
    Serial.print("ECG Values = ");
    Serial.print((float)ecg);
    Serial.println(" ");
    
    int hp = random(70, 72);
    Serial.print("Heart pulse = ");
    Serial.print((int)hp);
    Serial.println(" ");
    
    int bp = random(100, 102);
    Serial.print("Blood Pressre = ");
    Serial.print((int)bp);
    Serial.println(" ");
    delay(5000);
   
    if (! Temp.publish(temp))   //Publish to Adafruit
    {                     
        Serial.println(F("Failed to publish Temperature"));
    } 
    if (! Ecg.publish(ecg))       //Publish to Adafruit 
    {                     
        Serial.println(F("Failed to publish ECG"));
    }
    if (! BP.publish(bp))  //Publish to Adafruit 
    {                     
        Serial.println(F("Failed to publish Blood Pressure"));
    }
    if (! HP.publish(hp))  //Publish to Adafruit 
    {                     
        Serial.println(F("Failed to publish Heart pulse"));
    }
    else 
    {
        Serial.println(F("Sent!"));
        Serial.println("-------------------------------------------------------");
    }

    if((int)temp < 30 || temp > 37 )
    {
         digitalWrite(Buzzer_pin, HIGH);
         delay(1000);
         digitalWrite(Buzzer_pin, LOW);  
    }
}
