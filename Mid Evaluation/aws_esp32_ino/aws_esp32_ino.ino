/*
This code is to get multiple temp and humidity  data using Esp32 and NCD long-range-iot-wireless-temperature-humidity-sensor.
Also to create  AWS email alert on multiple data of sensor.
Sensor data structure can be found here https://ncd.io/long-range-iot-wireless-temperature-humidity-sensor-product-manual/
Sensor can be found here https://store.ncd.io/product/industrial-long-range-wireless-temperature-humidity-sensor/
*/

/* Include Libraries */
#include<WiFi.h>
#include<AWS_IOT.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>

#define WIFI_SSID "Redmi 5A"      //your wifi ssid
# define WIFI_PASSWD "k.pallavi"  //your wifi password
# define CLIENT_ID "xxxxxx"   // thing unique ID, can be any unique id
# define MQTT_TOPIC "xxxxx"   //topic for the MQTT data
# define AWS_HOST "xxxxx"     // your host for uploading data to AWS

uint8_t data[29];
int k = 10;
int i;
int temp;          // Assign the variable Temperature to store cTemp in (C)
int Humidity;      // Assign the variable Humidity
char payload[40];
AWS_IOT aws;       // AWS_IOT instance

void setup() 
{
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);

    Serial1.begin(115200, SERIAL_8N1, 16, 17); // pins 16 rx2, 17 tx2, 19200 bps, 8 bits no parity 1 stop bit
    Serial.begin(9600);
    Serial.println("ncd.io IoT ESP32 ESP8266 Temperature and Humidity sensor");

    Serial.print("\n  Initializing WIFI: Connecting to ");
    Serial.print("  ");

    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");
        delay(500);
    }

    //Initializing connetction to AWS
    Serial.println("\n  Connected.\n  Done");
    Serial.println("\n  Initializing connetction to AWS....");
  
    if (aws.connect(AWS_HOST, CLIENT_ID) == 0)  // connects to host and returns 0 upon success 
    {           
        Serial.println("  Connected to AWS\n  Done.");
    } 
    else 
    {
        Serial.println("  Connection failed!");
    }
    Serial.println("  Done.\n\nDone.\n");
}

void loop() 
{

    /*Reading sensor data */
    if (Serial1.available()) 
    {
        data[0] = Serial1.read();
        delay(k);
        if (data[0] == 0x7E) 
        {
            while (!Serial1.available());
                for (i = 1; i < 29; i++) 
                {
                    data[i] = Serial1.read();
                    delay(1);
                }
            if (data[15] == 0x7F)         // to check if the recive data is correct
            {
                if (data[22] == 1)          // make sure the sensor type is correct
                {
                    float humidity = ((((data[24]) * 256) + data[25]) / 100.0);
                    int16_t cTempint = (((uint16_t)(data[26]) << 8) | data[27]);
                    float cTemp = (float) cTempint / 100.0;
                    float fTemp = cTemp * 1.8 + 32;
                    float battery = ((data[18] * 256) + data[19]);
                    float voltage = 0.00322 * battery;
          
                    Serial.print("Sensor Number  ");
                    Serial.println(data[16]);
                    Serial.print("Sensor Type  ");
                    Serial.println(data[22]);
                    Serial.print("Firmware Version  ");
                    Serial.println(data[17]);
                    Serial.print("Relative Humidity :");
                    Serial.print(humidity);
                    Serial.println(" %RH");
                    Serial.print("Temperature in Celsius :");
                    Serial.print(cTemp);
                    Serial.println(" C");
                    Serial.print("Temperature in Fahrenheit :");
                    Serial.print(fTemp);
                    Serial.println(" F");
                    Serial.print("ADC value:");
                    Serial.println(battery);
                    Serial.print("Battery Voltage:");
                    Serial.print(voltage);
                    Serial.println("\n");
          
                    if (voltage < 1) 
                    {
                        Serial.println("Time to Replace The Battery");
                    }
                    temp = cTemp;          //Assign sensor cTemp(C) evaluated to temp variable
                    Humidity = humidity;   //Assign sensor humidity evaluated to Humidity variable
                }
            }     
            else 
            {
                for (i = 0; i < 29; i++) 
                {
                    Serial.print(data[i]);
                    Serial.print(" , ");
                    delay(1);
                }
            }
        }
    }

    // check whether reading was successful or not
    if (temp == NAN || Humidity == NAN)   // NAN means no available data 
    {         
        Serial.println("Reading failed.");
    } 
    else 
    {
        String temp_humidity = "Temperature: ";     //create string payload for publishing
        temp_humidity += String(temp);
        temp_humidity += " °C Humidity: ";
        temp_humidity += String(Humidity);
        temp_humidity += " %";

        temp_humidity.toCharArray(payload, 40);

        Serial.println("Publishing:- ");
        Serial.println(payload);
        if (aws.publish(MQTT_TOPIC, payload) == 0)   // publishes payload and returns 0 upon success 
        { 
            Serial.println("Success\n");
        } 
        else 
        {
            Serial.println("Failed!\n");
        }
    }
    delay(1000);
}
